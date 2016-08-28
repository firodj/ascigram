import os
import subprocess

TEST_ROOT = os.path.dirname(__file__)
PROJECT_ROOT = os.path.dirname(TEST_ROOT)
ASCIGRAM = [os.path.abspath(os.path.join(PROJECT_ROOT, 'ascigram'))]

tests = []

def collect_tests():
    for file in os.listdir(TEST_ROOT):
        if '.svg' in file:
            name = file.rsplit('.', 1)[0]
            tests.append(name)

    print("Collecting {0} tests.".format(len(tests)))

def main():
    collect_tests()

    flags = ['-T']

    for test in tests:
        cmd = ASCIGRAM + flags + [os.path.join(TEST_ROOT, test + '.txt')]
        print("Execute: {0}".format(cmd))

        ascigram_proc = subprocess.Popen(cmd, stdout=subprocess.PIPE)
        stdoutdata = ascigram_proc.communicate()[0]


if __name__ == '__main__':
    main()
