import random
from pathlib import Path
random.seed(13371337)

def gen_test(n):
    w = [random.randint(1, 100) for i in range(n)]
    p = [el + 10 for el in w]
    c = sum(w) // 2
    return (p, w, c)

def write_test(filename, p, w, c):
    n = len(p)
    with open(filename, 'w') as fout:
        print(n, c, file=fout)
        for i in range(n):
            print(p[i], w[i], file=fout)


Path('./tests_200').mkdir(parents=True, exist_ok=True)
for i in range(10):
    write_test('tests_200/{}.in'.format(i), *gen_test(100))
