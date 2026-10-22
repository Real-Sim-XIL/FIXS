"""Build the SDK-free C++ test programs when they are missing or stale.

The Python/C++ parity tests need the C++ side to have been compiled FROM THE
CODE AS IT IS NOW. They used to skip when it had not been, and a skip reads, in a
green run, exactly like a pass: test_cpp_golden_is_current skipped in every run
anyone looked at, so the comparison had been against a snapshot committed with
#325 and never against the C++ core -- which in the meantime stopped agreeing
with Python about which vehicle it was spawning, and nothing noticed.

So this builds them. It skips only when there is nothing to build with (not
Windows, or no MSVC), and says so. A build that fails is a FAILURE, with the
compiler's output, not a skip: it means the C++ core no longer compiles, which
is the most important thing a parity test can report.
"""

import glob
import hashlib
import os
import subprocess
import sys

import pytest

REPO_ROOT = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', '..'))
CPP_DIR = os.path.join(REPO_ROOT, 'tests', 'VirEnvCore')
BUILD_BAT = os.path.join(CPP_DIR, 'build_and_run.bat')
VSWHERE = os.path.join(os.environ.get('ProgramFiles(x86)', r'C:\Program Files (x86)'),
                       'Microsoft Visual Studio', 'Installer', 'vswhere.exe')

EXES = {name: os.path.join(CPP_DIR, name + '.exe')
        for name in ('smoke_interface', 'replay_core', 'blueprint_pick', 'view_dump')}

_COMMONLIB = os.path.join(REPO_ROOT, 'CommonLib')

#: What the programs are compiled from. An exe older than any of these is
#: answering for code that no longer exists.
def _sources():
    out = glob.glob(os.path.join(CPP_DIR, '*.cpp')) + glob.glob(os.path.join(CPP_DIR, '*.h'))
    out += [os.path.join(_COMMONLIB, f) for f in (
        'IVirEnvBackend.h', 'VirEnvCore.cpp', 'VirEnvCore.h', 'BlueprintPick.h',
        'MsgHelper.cpp', 'MsgHelper.h', 'SocketHelper.cpp', 'SocketHelper.h',
        'ConfigHelper.cpp', 'ConfigHelper.h', 'VehDataMsgDefs.h')]
    out.append(BUILD_BAT)
    return [p for p in out if os.path.isfile(p)]


#: Written after a successful build: a hash of every source's CONTENT. Content,
#: not modification times -- measured, a header restored with `mv` kept its old
#: mtime, looked older than the exe, and the exe built from the broken header
#: was reused. A checkout of an older branch does the same thing.
STAMP = os.path.join(CPP_DIR, 'build_stamp.txt')


def _sourceHash():
    h = hashlib.sha256()
    for p in sorted(_sources()):
        h.update(os.path.relpath(p, REPO_ROOT).encode('utf-8') + b'\0')
        with open(p, 'rb') as f:
            h.update(f.read())
        h.update(b'\0')
    return h.hexdigest()


def _stale():
    if not all(os.path.isfile(p) for p in EXES.values()):
        return True
    try:
        with open(STAMP, encoding='ascii') as f:
            return f.read().strip() != _sourceHash()
    except OSError:
        return True


_built = False


def ensureBuilt():
    """Build the programs once per session if they are missing or stale.

    Returns the ``{name: exe path}`` map. Skips only for want of a compiler.
    """
    global _built
    if _built:
        return EXES
    if sys.platform != 'win32':
        pytest.skip('the C++ side of the parity check is built with MSVC; not on %s'
                    % sys.platform)
    if not os.path.isfile(VSWHERE):
        pytest.skip('no MSVC on this machine (vswhere not found at %s), so the C++ '
                    'side of the parity check cannot be built here' % VSWHERE)
    if _stale():
        proc = subprocess.run(['cmd', '/c', BUILD_BAT], cwd=CPP_DIR,
                              stdin=subprocess.DEVNULL, capture_output=True, text=True)
        if proc.returncode != 0:
            pytest.fail('tests/VirEnvCore/build_and_run.bat failed (exit %d) -- the '
                        'C++ side does not build or its own guard failed:\n%s\n%s'
                        % (proc.returncode, proc.stdout[-6000:], proc.stderr[-2000:]),
                        pytrace=False)
        assert all(os.path.isfile(p) for p in EXES.values()), (
            'build_and_run.bat reported success but did not produce every exe')
        with open(STAMP, 'w', encoding='ascii') as f:
            f.write(_sourceHash())
    _built = True
    return EXES
