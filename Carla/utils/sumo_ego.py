"""
sumo_ego.py - put an application's ego vehicle into a map's SUMO scenario,
without the application owning a copy of that scenario.

A co-sim application needs one thing the map bundle cannot ship: its own ego, on
its own route, departing when the experiment says. The obvious way to get it is
to copy the bundle's route file, edit a route inside it, and run the copy - and
that is the wrong way round. The copy is map data the application now owns, so it
goes stale the moment the bundle is updated, and every change to the map has to
be mirrored by hand. MLK reached 1.8 MB of duplicated network and demand, and a
71-edge route list pasted into a Python file beside the route file it came from.

SUMO reads a LIST of route files. So the ego does not need to live in the map's:

    <route-files value="<the bundle's, untouched>,ego.rou.xml"/>

This writes that second file - vType, route, vehicle, nothing else, typically
under a kilobyte - plus a generated .sumocfg that points at the bundle in place
and sends the run's outputs to a run directory. The bundle is never copied and
never edited.

The generated .sumocfg path is printed on the last line, which is what an app
reports to run_cosim through FIXS_HANDOFF.

Stdlib only.

Usage
-----
  python sumo_ego.py --sumocfg ~/.fixs/maps/<map>/sumo/<map>.sumocfg \\
                     --out-dir MPR/run_0828 \\
                     --route-edges "51066109#1 E1 51066109#3" \\
                     --depart 29100 --repeat 20 \\
                     --type-from EGO_TYPE --speed-factor 1.1273 --accel 2.0

  # or take the route from one already in the bundle, by id
  python sumo_ego.py --sumocfg ... --out-dir ... --route-from route1 --depart 29100
"""
from __future__ import annotations

import argparse
import copy
import os
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

DEFAULT_EGO_ID = "ego"
DEFAULT_TYPE_ID = "EGO_TYPE_EXTERNAL"

# Outputs a run wants in its own directory rather than on top of the last run's.
# Every one is a <sumocfg><output> child except the timed event, which hides in
# <report>; both are rewritten to absolute paths under --out-dir.
OUTPUT_ELEMENTS = ("summary-output", "tripinfo-output", "fcd-output",
                   "netstate-dump", "emission-output", "full-output",
                   "queue-output", "statistic-output", "log")


def _split_list(value):
    return [p.strip() for p in (value or "").split(",") if p.strip()]


def _abs_from(base_dir, name):
    """A sumocfg's file reference resolved against the config that named it."""
    p = Path(name)
    return str(p if p.is_absolute() else (base_dir / p).resolve())


def route_from_bundle(route_files, route_id):
    """The edge list of an existing route, by id, from the bundle's demand.

    Lets an app say "the ego drives the corridor route the map already defines"
    instead of pasting that route into its own source, where it becomes a second
    copy that nothing keeps in step.
    """
    for rf in route_files:
        for _, el in ET.iterparse(rf, events=("end",)):
            if el.tag == "route" and el.get("id") == route_id:
                return (el.get("edges") or "").split()
            if el.tag in ("route", "vehicle", "flow"):
                el.clear()
    raise SystemExit(f"no route id '{route_id}' in: {', '.join(route_files)}")


def vtype_from_bundle(route_files, type_id):
    """A copy of a vType the bundle defines, so the ego inherits the calibrated
    car-following parameters instead of re-stating them."""
    for rf in route_files:
        for _, el in ET.iterparse(rf, events=("end",)):
            if el.tag == "vType" and el.get("id") == type_id:
                return copy.deepcopy(el)
            if el.tag in ("vType", "vehicle", "flow", "route"):
                el.clear()
    raise SystemExit(f"no vType id '{type_id}' in: {', '.join(route_files)}")


def build_ego_routes(opt, route_files):
    """The app's own route file: a vType, a route, and one vehicle."""
    root = ET.Element("routes")

    if opt.type_from:
        vtype = vtype_from_bundle(route_files, opt.type_from)
        vtype.set("id", opt.type_id)
    else:
        vtype = ET.Element("vType", {"id": opt.type_id, "vClass": "passenger"})
    for attr, value in (("accel", opt.accel), ("decel", opt.decel),
                        ("speedFactor", opt.speed_factor), ("speedDev", opt.speed_dev)):
        if value is not None:
            vtype.set(attr, str(value))
        else:
            # An inherited value must be droppable, or "use SUMO's default" is
            # unreachable for any app whose bundle vType happens to set it.
            vtype.attrib.pop(attr, None)
    root.append(vtype)

    edges = (opt.route_edges.split() if opt.route_edges
             else route_from_bundle(route_files, opt.route_from))
    if not edges:
        raise SystemExit("the ego route is empty")
    if opt.repeat > 0 and len(edges) > 1 and edges[0] == edges[-1]:
        # `repeat` concatenates whole edge lists, so a lap that names its start
        # edge again at the end would make an A -> A transition between laps.
        edges = edges[:-1]
    route = ET.SubElement(root, "route", {"id": opt.route_id, "edges": " ".join(edges)})
    if opt.repeat > 0:
        route.set("repeat", str(opt.repeat))

    veh = {"id": opt.ego_id, "type": opt.type_id, "route": opt.route_id,
           "depart": str(opt.depart), "departLane": opt.depart_lane,
           "departPos": opt.depart_pos, "departSpeed": str(opt.depart_speed)}
    ET.SubElement(root, "vehicle", veh)
    return ET.ElementTree(root), len(edges)


def _substitutions(pairs):
    """--replace as {basename: absolute replacement}, checked for existence."""
    out = {}
    for spec in pairs or []:
        name, sep, path = spec.partition("=")
        if not sep or not name.strip() or not path.strip():
            raise SystemExit(f"--replace wants NAME=PATH, got '{spec}'")
        repl = Path(path.strip()).resolve()
        if not repl.is_file():
            raise SystemExit(f"--replace target does not exist: {repl}")
        out[name.strip()] = str(repl)
    return out


def _apply_replacements(files, repl, used):
    """Swap any file the caller is substituting, by basename."""
    out = []
    for f in files:
        name = Path(f).name
        if name in repl:
            used.add(name)
            out.append(repl[name])
        else:
            out.append(f)
    return out


def _set_value(root, section, tag, value):
    """<section><tag value="..."/></section>, created if absent.

    `x = root.find(t) or ET.SubElement(...)` is wrong here: an Element with no
    children is FALSY, so an existing childless node would be replaced by a second
    one and SUMO refuses the config as "defined twice".
    """
    sec = root.find(section)
    if sec is None:
        sec = ET.SubElement(root, section)
    node = sec.find(tag)
    if node is None:
        node = ET.SubElement(sec, tag)
    node.set("value", str(value))


def demand_file_with(route_files, route_id):
    """The route file that defines `route_id` - the bundle's demand."""
    for f in route_files:
        for _, el in ET.iterparse(f, events=("end",)):
            if el.tag == "route" and el.get("id") == route_id:
                return f
            if el.tag in ("route", "vehicle", "flow"):
                el.clear()
    return None


def _insert_by_depart(root, vehicle):
    """Before the first vehicle that departs later; else at the end.

    The POSITION is the point. SUMO draws each vehicle's speedFactor as it is
    created, from one stream, so a vehicle's index decides which number every
    vehicle after it gets. Appending the ego instead of placing it in depart order
    moves ~8000 draws by one slot and the whole background traffic changes -
    measured: ego at index 758 vs last diverges at t=29109.2 on a vehicle 522 m
    away, which nothing physical connects to the ego.
    """
    depart = float(vehicle.get("depart", "inf"))
    for i, child in enumerate(list(root)):
        if child.tag != "vehicle":
            continue
        d = child.get("depart")
        if d is not None and float(d) > depart:
            root.insert(i, vehicle)
            return i
    root.append(vehicle)
    return len(list(root)) - 1


def inject_ego(opt, demand, out_path, vtype, edges):
    """A copy of `demand` with the ego's vType, route and vehicle written into it.

    Mirrors what prepare_scenario.py does, because matching it is the whole
    purpose: vType first, the named route reused in place, vehicle in depart
    order. Returns the path written.
    """
    tree = ET.parse(demand)
    root = tree.getroot()

    if vtype is not None:
        root.insert(0, vtype)

    route = root.find(f"./route[@id='{opt.route_from}']") if opt.route_from else None
    if route is None:
        route = ET.Element("route", {"id": opt.route_id})
        root.insert(0, route)
    route.set("edges", " ".join(edges))
    if opt.repeat > 0:
        route.set("repeat", str(opt.repeat))

    veh = ET.Element("vehicle", {
        "id": opt.ego_id, "type": opt.type_id, "route": route.get("id"),
        "depart": str(opt.depart), "departLane": opt.depart_lane,
        "departPos": opt.depart_pos, "departSpeed": str(opt.depart_speed)})
    idx = _insert_by_depart(root, veh)

    out_path = Path(out_path)
    out_path.parent.mkdir(parents=True, exist_ok=True)
    tree.write(out_path, encoding="UTF-8", xml_declaration=True)
    n = sum(1 for _ in root.iter("vehicle"))
    print(f"[sumo_ego] ego injected into a copy of {Path(demand).name} at element "
          f"{idx} of {n} vehicles -> {out_path}")
    return str(out_path)


def build_sumocfg(opt, base_cfg, ego_rou, replace_demand=None):
    """The bundle's config, with the ego file appended and outputs redirected.

    Everything the bundle names is rewritten to an ABSOLUTE path so the generated
    config can live in the run directory while the network and demand stay in the
    map cache. That is the whole point: nothing is copied.

    --replace NAME=PATH swaps one of those files for the caller's own, by base
    name. It exists because some of what a bundle ships is a DEFAULT rather than a
    fact: the vType file carries the map's calibrated type mix, and a study that
    wants a different one would otherwise have to copy the demand to change two
    attributes. Substituting a few-hundred-byte file leaves the demand untouched
    and still uncopied.
    """
    base_dir = base_cfg.parent
    tree = ET.parse(base_cfg)
    root = tree.getroot()
    inp = root.find("input")
    if inp is None:
        raise SystemExit(f"{base_cfg} has no <input> section")

    repl = _substitutions(opt.replace)
    used = set()

    for tag in ("net-file", "additional-files"):
        node = inp.find(tag)
        if node is not None and node.get("value"):
            files = [_abs_from(base_dir, f) for f in _split_list(node.get("value"))]
            node.set("value", ",".join(_apply_replacements(files, repl, used)))

    routes = inp.find("route-files")
    if routes is None or not routes.get("value"):
        raise SystemExit(f"{base_cfg} names no route-files")
    bundle_routes = _apply_replacements(
        [_abs_from(base_dir, f) for f in _split_list(routes.get("value"))], repl, used)
    if replace_demand:
        # The ego is INSIDE this copy, so the demand entry is swapped for it and no
        # second route file is appended - one vehicle-bearing file, as before.
        demand, injected = replace_demand
        bundle_routes = [injected if os.path.samefile(f, demand) else f
                         for f in bundle_routes]
        routes.set("value", ",".join(bundle_routes))
    else:
        routes.set("value", ",".join(bundle_routes + [str(ego_rou)]))

    # A --replace that matched nothing is a typo, and a silent one would leave the
    # bundle's own file running while the caller believes theirs is.
    missing = sorted(set(repl) - used)
    if missing:
        raise SystemExit(f"{base_cfg} names no input file called "
                         f"{', '.join(missing)}; --replace matched nothing")

    # The scenario's own SUMO settings. These belong in the generated config, not
    # on the runner's command line: a value declared per-app there overrides
    # whatever the scenario says, silently, so the two drift and the flag wins.
    # (FIXS_Applications#45: a restated --time-to-teleport 30 overrode a generated
    # config's 150 and walked an app 12.75 m/s off its reference results.)
    if opt.end is not None:
        _set_value(root, "time", "end", opt.end)
    if opt.step_length is not None:
        _set_value(root, "time", "step-length", opt.step_length)
    if opt.time_to_teleport is not None:
        _set_value(root, "processing", "time-to-teleport", opt.time_to_teleport)
    if opt.seed is not None:
        _set_value(root, "random_number", "seed", opt.seed)

    for section in ("output", "report"):
        sec = root.find(section)
        if sec is None:
            continue
        for node in list(sec):
            if node.tag in OUTPUT_ELEMENTS and node.get("value"):
                node.set("value", str(opt.out_dir / Path(node.get("value")).name))
            for ev in node.iter():
                if ev.get("dest"):
                    ev.set("dest", str(opt.out_dir / Path(ev.get("dest")).name))
    return tree, bundle_routes


def build_parser():
    p = argparse.ArgumentParser(
        description="Add an app's ego to a map's SUMO scenario without copying it.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__.split("Usage\n-----\n")[-1])
    p.add_argument("--sumocfg", required=True, help="the map bundle's .sumocfg (read only)")
    p.add_argument("--out-dir", required=True, help="run directory for the generated files and outputs")

    r = p.add_argument_group("the ego's route (one of these)")
    r.add_argument("--route-edges", help="space-separated edge ids")
    r.add_argument("--route-from", metavar="ROUTE_ID",
                   help="take the edges from a route already defined - in the bundle's "
                        "demand, or in any --route-file the app ships")
    r.add_argument("--route-file", action="append", default=[], metavar="ROU.XML",
                   help="an app's own route file to search for --route-from / --type-from "
                        "(repeatable). Lets the ego's route be shipped as data next to the "
                        "app instead of pasted into its code")
    r.add_argument("--repeat", type=int, default=0,
                   help="drive the route this many extra times (0 = once)")

    e = p.add_argument_group("the ego")
    e.add_argument("--id", dest="ego_id", default=DEFAULT_EGO_ID)
    e.add_argument("--route-id", default="ego_route")
    e.add_argument("--type-id", default=DEFAULT_TYPE_ID)
    e.add_argument("--type-from", metavar="VTYPE_ID",
                   help="inherit the bundle's calibrated vType of this id")
    e.add_argument("--depart", type=float, required=True)
    e.add_argument("--depart-lane", default="first")
    e.add_argument("--depart-pos", default="free")
    e.add_argument("--depart-speed", default="0.1")
    e.add_argument("--accel", type=float, default=None)
    e.add_argument("--decel", type=float, default=None)
    e.add_argument("--speed-factor", type=float, default=None,
                   help="omit to keep the inherited/default distribution")
    e.add_argument("--speed-dev", type=float, default=None)

    o = p.add_argument_group("run")
    o.add_argument("--end", type=float, default=None, help="override the scenario end time")
    o.add_argument("--step-length", type=float, default=None,
                   help="SUMO step length written into the generated config")
    o.add_argument("--time-to-teleport", type=float, default=None,
                   help="seconds a blocked vehicle waits before SUMO teleports it. "
                        "Set it here rather than as a per-app SUMO flag: a flag "
                        "overrides the scenario silently, and the two then drift")
    o.add_argument("--seed", type=int, default=None,
                   help="SUMO random seed written into the generated config")
    o.add_argument("--inject", metavar="PATH", default=None,
                   help="write the ego INTO a copy of the bundle's demand at PATH, at "
                        "its depart position, instead of adding it as a second route "
                        "file. Costs one copy of the demand; buys a run that matches "
                        "one built by editing the demand directly, because a "
                        "vehicle's index decides which speedFactor every later "
                        "vehicle draws")
    o.add_argument("--replace", action="append", default=[], metavar="NAME=PATH",
                   help="swap one of the bundle's input files for your own, by base "
                        "name (repeatable). For what a bundle ships as a default "
                        "rather than a fact - its vType file, say - so a run can "
                        "differ from it without copying the demand")
    o.add_argument("--name", default=None,
                   help="basename for the generated files (default: the bundle cfg's stem + _ego)")
    return p


def main(argv=None):
    opt = build_parser().parse_args(argv)
    if bool(opt.route_edges) == bool(opt.route_from):
        raise SystemExit("give exactly one of --route-edges or --route-from")

    base_cfg = Path(opt.sumocfg).resolve()
    if not base_cfg.is_file():
        raise SystemExit(f"no such sumocfg: {base_cfg}")
    opt.out_dir = Path(opt.out_dir).resolve()
    opt.out_dir.mkdir(parents=True, exist_ok=True)

    stem = opt.name or (base_cfg.stem + "_ego")
    ego_rou = opt.out_dir / f"{stem}.rou.xml"
    out_cfg = opt.out_dir / f"{stem}.sumocfg"

    # route-files are needed before the ego file exists, to read a vType or route
    # out of the bundle
    base_dir = base_cfg.parent
    rf_node = ET.parse(base_cfg).getroot().find("input/route-files")
    if rf_node is None or not rf_node.get("value"):
        raise SystemExit(f"{base_cfg} names no route-files")
    bundle_routes = [_abs_from(base_dir, f) for f in _split_list(rf_node.get("value"))]

    # the app's own route files are searched FIRST: an ego route the app ships is
    # its own statement of intent and must win over a same-named route in the map
    search = [str(Path(f).resolve()) for f in opt.route_file] + bundle_routes
    for f in search[:len(opt.route_file)]:
        if not Path(f).is_file():
            raise SystemExit(f"no such --route-file: {f}")
    routes_tree, n_edges = build_ego_routes(opt, search)

    replace_demand = None
    if opt.inject:
        demand = demand_file_with(bundle_routes, opt.route_from) if opt.route_from else None
        if demand is None:
            demand = bundle_routes[-1]
        # Reuse exactly what build_ego_routes resolved, so --inject and the default
        # differ ONLY in where the ego is written, never in what the ego is.
        built = routes_tree.getroot()
        vt = built.find("vType")
        edges = built.find("route").get("edges").split()
        injected = inject_ego(opt, demand, opt.inject, vt, edges)
        replace_demand = (demand, injected)
    else:
        routes_tree.write(ego_rou, encoding="UTF-8", xml_declaration=True)

    cfg_tree, _ = build_sumocfg(opt, base_cfg, ego_rou, replace_demand=replace_demand)
    cfg_tree.write(out_cfg, encoding="UTF-8", xml_declaration=True)

    print(f"[sumo_ego] bundle   {base_cfg}   (not copied, not modified)"
          if not opt.inject else
          f"[sumo_ego] bundle   {base_cfg}   (net/signals not copied; demand copied)")
    if not opt.inject:
        print(f"[sumo_ego] ego      {opt.ego_id} on {n_edges} edges, depart {opt.depart:g}"
              + (f", repeat {opt.repeat}" if opt.repeat else "")
              + f"  -> {ego_rou.name} ({ego_rou.stat().st_size} bytes)")
    print(out_cfg)
    return 0


if __name__ == "__main__":
    sys.exit(main())
