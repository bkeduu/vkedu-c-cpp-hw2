import numpy as np
import time
import threading

def gen_file(cnt, rmax, fnum):
    arr = np.random.randint(0, rmax, (cnt, 1), dtype=int)
    curr_time = int(time.time())
    np.savetxt('vec_{}_{}.txt'.format(curr_time, fnum), arr, delimiter='\n', fmt='%d')
    print('Saved file vec_{}_{}.txt with {} random integers'.format(curr_time, fnum, cnt))


if __name__ == '__main__':
    count = int(input("Input number of generated integers: "))
    rand_max = int(input("Input maximum for random generation: "))
    files_count = int(input("Enter count of files: "))

    for i in range(files_count):
        t = threading.Thread(target=gen_file, args=[count, rand_max, i])
        t.start()