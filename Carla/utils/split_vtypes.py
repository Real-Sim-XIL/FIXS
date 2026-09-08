"""
split_vtypes.py - lift the vehicle types out of a scenario's demand file.

A demand file names the traffic: the routes and the vehicles that drive them. By
habit it also carries the <vType> definitions those vehicles refer to, and that
is what makes a type parameter expensive to change. Changing one attribute -
the share of a distribution, a car-following term, an ego type's acceleration -
means rewriting a file that may hold tens of thousands of <vehicle> elements.
Any consumer that wants a different type mix from the one a map ships therefore
ends up copying the whole demand, and a copied demand goes stale the moment the
map is updated.

Split, the two come apart. The demand stays where it is and is pointed at in
place; the types become a small file a caller can regenerate and substitute. It
is the same idea sumo_ego.py applies to the ego: add a second file rather than
edit the map's own.

The types are written as a ROUTE file and named FIRST in the config's
route-files. SUMO accepts them in an additional-file too, but route-files are
loaded in the order given, so this states the load order instead of relying on
how the two lists interleave - and it keeps the types findable by anything that
reads a scenario's route-files to inherit a calibrated vType, which is what
sumo_ego.py --type-from does.

Reversible: concatenating the two files' children back into one <routes> element
gives the original demand, and SUMO cannot tell the difference. Confirm that on
a scenario you split - a short run before and after should produce identical
tripinfo and fcd output.

Usage
-----
  python split_vtypes.py --routes sumo/routes.rou.xml --out sumo/vtypes.rou.xml
  python split_vtypes.py --routes ... --out ... --sumocfg sumo/map.sumocfg

With --sumocfg the config's route-files is rewritten to name the new file first.
Stdlib only.
"""
from __future__ import annotations

import argparse
import sys
import xml.etree.ElementTree as ET
from pathlib import Path

# Everything that DEFINES a type, as opposed to using one. vTypeDistribution
# comes along because it names vTypes and is meaningless apart from them.
TYPE_TAGS = ("vType", "vTypeDistribution")


def split(routes_path: Path, out_path: Path) -> tuple[int, int]:
    """Move the type definitions from `routes_path` into `out_path`.

    Returns (types moved, elements left in the demand). The demand is rewritten
    in place with the type elements removed and everything else - order
    included - left exactly as it was.
    """
    tree = ET.parse(routes_path)
    root = tree.getroot()
    types = [el for el in list(root) if el.tag in TYPE_TAGS]
    if not types:
        sys.exit(f"[split_vtypes] {routes_path} defines no {'/'.join(TYPE_TAGS)}")

    out_root = ET.Element(root.tag, root.attrib)
    for el in types:
        out_root.append(el)
        root.remove(el)
    ET.ElementTree(out_root).write(out_path, encoding="UTF-8", xml_declaration=True)
    tree.write(routes_path, encoding="UTF-8", xml_declaration=True)
    return len(types), len(list(root))


def name_in_sumocfg(sumocfg_path: Path, vtypes_name: str) -> None:
    """Put `vtypes_name` first in the config's route-files, once."""
    tree = ET.parse(sumocfg_path)
    node = tree.getroot().find("input/route-files")
    if node is None or not node.get("value"):
        sys.exit(f"[split_vtypes] {sumocfg_path} names no route-files")
    files = [f.strip() for f in node.get("value").split(",") if f.strip()]
    if vtypes_name in files:
        files.remove(vtypes_name)
    node.set("value", ",".join([vtypes_name] + files))
    tree.write(sumocfg_path, encoding="UTF-8", xml_declaration=True)
    print(f"[split_vtypes] {sumocfg_path.name}: route-files = {node.get('value')}")


def build_parser() -> argparse.ArgumentParser:
    p = argparse.ArgumentParser(
        description="Lift a scenario's vehicle types into their own route file.",
        formatter_class=argparse.RawDescriptionHelpFormatter,
        epilog=__doc__.split("Usage\n-----\n")[-1])
    p.add_argument("--routes", required=True, type=Path,
                   help="the demand file to lift the types out of (rewritten in place)")
    p.add_argument("--out", required=True, type=Path,
                   help="the type file to write")
    p.add_argument("--sumocfg", type=Path,
                   help="also name the type file first in this config's route-files")
    return p


def main() -> int:
    opt = build_parser().parse_args()
    if not opt.routes.is_file():
        sys.exit(f"[split_vtypes] no such route file: {opt.routes}")
    moved, left = split(opt.routes, opt.out)
    print(f"[split_vtypes] {opt.out.name}: {moved} type definition(s), "
          f"{opt.out.stat().st_size} bytes")
    print(f"[split_vtypes] {opt.routes.name}: {left} element(s) left, "
          f"{opt.routes.stat().st_size} bytes")
    if opt.sumocfg:
        name_in_sumocfg(opt.sumocfg, opt.out.name)
    return 0


if __name__ == "__main__":
    sys.exit(main())
