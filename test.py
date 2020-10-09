#!/usr/bin/env python3
import os
import subprocess
import sys
import random
import argparse


parser = argparse.ArgumentParser(description='Test program time.')
parser.add_argument('-b', '--binary', help='binary to test', required=True, type=str)
parser.add_argument('-l', '--test-large', help='test large tests', action='store_true')
parser.add_argument('-m', '--max-threads', help='maximum threads count', default=8, type=int)

args = parser.parse_args()

binary = './' + args.binary
small_tests = ['small_tests/' + e for e in os.listdir('./small_tests') if e.endswith('.in')]
large_tests = ['tests_100/' + e for e in os.listdir('./tests_100') if e.endswith('.in')]

small_results = {}
large_results = {}

print('Small tests:')
for th_cnt in range(1, args.max_threads + 1):
    sm = 0
    cnt = 100
    for i in range(cnt):
        outp = subprocess.check_output([binary, str(th_cnt)], stdin=open(random.choice(small_tests)))
        sm += int(outp.split()[1])
    small_results[th_cnt] = sm / cnt
    print('Threads: {}, result: {} microseconds'.format(th_cnt, sm / cnt))

if args.test_large:
    print('Large tests:')
    for th_cnt in range(1, args.max_threads + 1):
        sm = 0
        cnt_each = 3
        cnt = len(large_tests) * cnt_each
        for t in large_tests:
            for i in range(cnt_each):
                outp = subprocess.check_output([binary, str(th_cnt)], stdin=open(t))
                sm += int(outp.split()[1])
        large_results[th_cnt] = sm / cnt
        print('Threads: {}, result: {} microseconds'.format(th_cnt, sm / cnt))

