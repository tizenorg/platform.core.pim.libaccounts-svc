#!/bin/sh

. ./_export_target_env.sh                    # setting environment variables

export TET_INSTALL_PATH=/mnt/nfs/TETWare
export TET_TARGET_PATH=$TET_INSTALL_PATH/tetware-target
export PATH=$TEST_TARGET_PATH/bin:$PATH
export LD_LIBRARY_PATH=$TET_TARGET_PATH/lib/tet3:$LD_LIBRARY_PATH
export TET_ROOT=$TET_TARGET_PATH
export TET_SUITE_ROOT=`pwd`
FILE_NAME_EXTENSION=`date +%s`

RESULT_DIR=results
HTML_RESULT=$RESULT_DIR/exec-tar-result-$FILE_NAME_EXTENSION.html
JOURNAL_RESULT=$RESULT_DIR/exec-tar-result-$FILE_NAME_EXTENSION.journal

mkdir -p $RESULT_DIR

tcc -e -j $JOURNAL_RESULT -p ./
grw -c 3 -f chtml -o $HTML_RESULT $JOURNAL_RESULT
