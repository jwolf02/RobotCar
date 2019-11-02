import numpy as np
import sys


EXIT_SUCCESS = 0
EXIT_FAILURE = -1


def create_network(args):
    pass


def train_network(args):
    pass


def export_network(args):
    pass


def main(argv):
    if len(argv) == 1:
        print('Usage:', argv[0], '<operation> [options...]')
        return EXIT_FAILURE

    operations = {'--create': create_network, '--train': train_network, '--export': export_network}
    op = argv[1]
    if op not in operations.keys():
        print("operation '", op, "' not recognized", sep='')
        return EXIT_FAILURE

    func = operations[op]
    func(argv[2:])

    return EXIT_SUCCESS


if __name__ == '__main__':
    ret = main(sys.argv)
    exit(ret)
