#!/usr/bin/env python3

import os
import sys
import glob
import filecmp
import tempfile
import shutil
import subprocess


jp = os.path.join


def bold(*args, **kwargs):
    print(end='\033[1m')
    try:
        print(*args, **kwargs)
    finally:
        print(end='\033[0;0m')


def verify_and_get_path(path):
    abspath = os.path.abspath(path)
    if os.path.exists(path):
        return abspath
    else:
        raise SystemExit('Error: {!r} does not exist'.format(path))


def validate_testcase(directory):
    if not os.path.exists(jp(directory, 'dimage.bin')):
        print('=> dimage not found in', directory)
    elif not os.path.exists(jp(directory, 'iimage.bin')):
        print('=> iimage not found in', directory)
    else:
        print('=> Found', directory + '/{i,d}image.bin')
        return directory


def run_simulator(name, executable, testcase):
    print('-' * 79)
    with tempfile.TemporaryDirectory() as wd:
        shutil.copy(jp(testcase, 'iimage.bin'), wd)
        shutil.copy(jp(testcase, 'dimage.bin'), wd)
        exitcode = subprocess.call(
            [executable],
            cwd=wd,
            # stdout=subprocess.DEVNULL,
            # stderr=subprocess.DEVNULL,
        )
        bold('=> {} returned {}'.format(name, exitcode))
        shutil.copy(
            os.path.join(wd, 'snapshot.rpt'),
            os.path.join(testcase, 'snapshot.rpt.{}'.format(name))
        )
        shutil.copy(
            os.path.join(wd, 'error_dump.rpt'),
            os.path.join(testcase, 'error_dump.rpt.{}'.format(name))
        )


def compare(prefix):
    goldfile = prefix + '.gold'
    userfile = prefix + '.user'
    equal = filecmp.cmp(goldfile, userfile)
    if equal:
        print('=> {:14} Passed'.format(os.path.basename(prefix)))
        return True
    if not equal:
        print('=> {:14} Differ'.format(os.path.basename(prefix)))
        subprocess.call(['colordiff', '-u', goldfile, userfile])
        return False


def judge(gold, user, testcase):
    print('=' * 79)
    print('Running testcase:', testcase)
    run_simulator('gold', gold, testcase)
    run_simulator('user', user, testcase)
    print('-' * 79)
    bold('Results:')
    snap = compare(jp(testcase, 'snapshot.rpt'))
    err = compare(jp(testcase, 'error_dump.rpt'))
    return snap and err


def main(directories):
    print('=' * 79)
    tadir = '../../archiTA'
    bold('Discovering implementations...')
    userrel = './single_cycle'
    goldrel = jp(tadir, 'simulator/single_cycle')
    print('=> Using user executable:', userrel)
    userabs = verify_and_get_path(userrel)
    print('=> Using golden executable:', goldrel)
    goldabs = verify_and_get_path(goldrel)
    bold('Discovering open test cases...')
    testcases = []
    testcases.extend(
        filter(
            None,
            map(
                validate_testcase,
                glob.iglob(jp(tadir, 'testcase', 'open_testcase', '*'))
            )
        )
    )
    bold('Discovering user test case...')
    usertest = validate_testcase('../testcase')
    if usertest is not None:
        testcases.append(usertest)
    bold('Discovering custom test cases...')
    if directories:
        testcases.extend(
            filter(
                None,
                map(validate_testcase, directories)
            )
        )
    else:
        print('=> No custom test cases specified')
    passed = 0
    for testcase in testcases:
        passed += judge(goldabs, userabs, testcase)
    print('=' * 79)
    bold('Summary:', passed, 'of', len(testcases), 'passed.')


if __name__ == '__main__':
    main(sys.argv[1:])
