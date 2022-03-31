import numpy as np
import threading

def gen_file(cnt, rmax, fnum):
    np.savetxt('vec_{}.txt'.format(fnum), np.random.randint(0, rmax, (cnt, 1), dtype=int), delimiter='\n', fmt='%d')
    print('Saved file vec_{}.txt with {} random integers'.format(fnum, cnt))


if __name__ == '__main__':
    count = int(input("Input number of generated integers: "))
    rand_max = int(input("Input maximum for random generation: "))
    files_count = int(input("Enter count of files: "))

    for i in range(files_count):
        threading.Thread(target=gen_file, args=[count, rand_max, i]).start()