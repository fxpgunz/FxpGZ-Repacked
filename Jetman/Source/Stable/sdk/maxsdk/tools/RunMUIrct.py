# Copyright (c) 1998-2016 Autodesk, Inc.
#
# All rights reserved.
# Use of this software is subject to the terms of the Autodesk license 
# agreement provided at the time of installation or download, or which 
# otherwise accompanies this software in either electronic or hard copy form.

import os
import sys
import argparse
import re
import glob
import tempfile
import shutil
import multiprocessing
import subprocess
import time

"""
The functions in this script extract resource strings embedded in dlls
in order to allow for localizing them using automated tools. 
The strings are extracted as .mui files using the RunMUIrct.exe utility.
The .mui files are created in a en-US subfolder of the folder where the dll is located.
"""

def StartProcessFile(program, temp_root, path):
    """
    Processes a single file by running RunMUIrct.exe
    Params:
        program: RunMUIrct.exe path
        temp_root: temporary root
        path: binary file to process
    Return:
        [ process status, process, temp directory, processing file ]
    """
    if os.path.isfile(path):
        tmp = tempfile.mkdtemp(dir=temp_root)
        params = [program, "-v", "0", "-o", os.path.dirname(path) + '\\', "-f", os.path.basename(path), "-t", tmp]
        return [1, subprocess.Popen(params), tmp, path]

def make_dir_unless_exists(path):
    """
    Creates a given path unless it exists already
    Params:
        path: the folder to be created
    Raises an exception if something went wrong, other then "path already exists"
    """
    try:
        os.makedirs(path)
    except OSError as exception:
        if exception.errno != errno.EEXIST:
            raise

def AddFile(process_file, file_name, file_dir):
    """
    Adds a file to the list of files to process
    Params:
      process_file: name of the file to process
      file_name: mangled name of the file used in the "file list"
      file_dir: directory that contains the "file list"

      Use of a directory with files to represent the file list eliminates the
      need to do file locking on a single file.
    """
    if not os.path.isdir(file_dir):
        make_dir_unless_exists(file_dir)
    fullname = os.path.join(file_dir, file_name)
    fo = open(fullname, 'w+')
    fo.write(process_file)
    fo.close()


def GenDummyFileName(file_name):
    """
    Generates a dummy file name by replacing ':', '\' and '.' with _
    Params:
        file_name: the file name
    """
    return re.sub("[:\\\\.]", '_', file_name) + '.txt'

def main():
    """
    Main routine
    """
    start_time = time.time()
    cmd_parser = argparse.ArgumentParser(description='RunMUIRct prep code'
                                                     '\nNOTE: use help to see options',
                                         formatter_class=argparse.RawTextHelpFormatter)
    cmd_parser.add_argument('--verbose', action='store_true', help='verbose info')
    cmd_parser.add_argument('--process-file', help='process a single file of name')
    cmd_parser.add_argument('--add-file', help='add the list of files to process')
    cmd_parser.add_argument('--file-dir', help='directory containing a dummy file for each file to process')
    cmd_parser.add_argument('--process-file-dir', help='process all the files listed in file-dir')
    cmd_parser.add_argument('--devel-dir', help='devel root directory used to find RunMUIrct.exe')
    cmd_arguments = cmd_parser.parse_args()

    if cmd_arguments.verbose:
        print "add_file %s" % cmd_arguments.add_file
        print "process_file %s" % cmd_arguments.process_file
        print "file_dir %s" % cmd_arguments.file_dir
        print "process_file_dir %s" % cmd_arguments.process_file_dir
        print "devel_dir %s" % cmd_arguments.devel_dir

    # Add a file to the list of files in the "file list" directory
    if cmd_arguments.add_file:
        if cmd_arguments.file_dir:
            file_name = GenDummyFileName(cmd_arguments.add_file)
            if not os.path.isfile(file_name):
                AddFile(cmd_arguments.add_file, file_name, cmd_arguments.file_dir)
            elif cmd_arguments.verbose:
                print "file already found: " + cmd_arguments.add_file
            return 0
        else:
            print("RunMUIrct.py: --file-dir must be specified")
            return 1

    if not cmd_arguments.devel_dir:
        print "--devel-dir must be specified"
        return 1

    devel_dir = os.path.normpath(cmd_arguments.devel_dir)
    temp_root = os.path.join(devel_dir, '3dswin', 'bin')
    program = os.path.join(devel_dir, '3dswin', 'src', 'maxsdk', 'tools', 'RunMUIrct.exe')

    # Process a single file
    if cmd_arguments.process_file:
        process_info = StartProcessFile(program, temp_root, cmd_arguments.process_file)
        process_info[0].wait()
        shutil.rmtree(process_info[1], True)
        return 0

    # process all the files in the "file list" directory
    if cmd_arguments.process_file_dir:
        if cmd_arguments.verbose:
            print("========== RunMUIrct.py Start ==========\nprocessing the files dir: %s" % cmd_arguments.process_file_dir)
        sys.stdout.flush()

        err_found = False

        # RunMUIrct workload of some files is very low
        # we could slightly increase the process pool
        available = int(multiprocessing.cpu_count() * 1.2)
        task_pool = []
        post_task = []
        file_list = glob.glob(os.path.join(cmd_arguments.process_file_dir, '*.txt'))
        for file_name in file_list:
            with open(file_name, 'r') as f:
                line = f.readline()

            # check process until pool available
            while not available:
                for task_info in task_pool:
                    if task_info[1].poll() is not None:
                        # set process info status to 0 (finished)
                        task_info[0] = 0
                        post_task.append(task_info)
                        available += 1

                # wait for a while
                if not available:
                    sys.stdout.flush()
                    time.sleep(0.1)
                else:
                    # record processes which still 'alive' in last check point
                    task_pool = filter(lambda x: x[0], task_pool)

            task_info = StartProcessFile(program, temp_root, line.rstrip('\n'))
            # if process startup success, delay delete fname to process finished
            if task_info:
                task_info.append(file_name)
                task_pool.append(task_info)
                available -= 1
            else:
                os.remove(file_name)
                pass

        # wait all processes to be finished
        for task_info in task_pool:
            task_info[1].wait()
            post_task.append(task_info)

        for post_task in post_task:
            if post_task[1].returncode != 0:
                print("RunMUIrct.py: WARNING - Possible problem processing: %s, %d " % (post_task[3], post_task[1].returncode))
                # err_found=True;  # Ignore possible errors for now
            else:
                os.remove(post_task[4])
                pass
            shutil.rmtree(post_task[2], True)

        if err_found:
            print("========== RunMUIrct.py -- ERROR! ========")
        elif cmd_arguments.verbose:
            print("========== RunMUIrct.py -- Done ==========")

        elapsed = int(time.time() - start_time)
        print("RunMUIrct Finished. Time Elapsed %d:%d:%d" % (elapsed/3600, (elapsed/60) % 60, elapsed % 60))

        sys.stdout.flush()
        return err_found


if __name__ == '__main__':
    exit(main())
