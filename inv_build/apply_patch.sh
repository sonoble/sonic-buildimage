#!/bin/bash
## This script is to apply inventec own patch to SONiC

BASE_PATH=$(pwd)
INV_PATCH_DIR=$BASE_PATH/inv_build/patch_files

if [ ! -d ${INV_PATCH_DIR}/* ]; then
    exit 0
fi

# Find each submodule need to patch
echo "[INFO] start to apply patch in different module"
for entry in $INV_PATCH_DIR/*
do
    if [ ! -d $entry ]; then
        continue
    fi

    SUB_MODULE="$(basename $entry)"

    if [ "$SUB_MODULE" = "README" ]; then
       continue
    elif [ "$SUB_MODULE" = "sonic-buildimage" ]; then
        echo
        echo "=== apply patch to $SUB_MODULE ==="
        seriesfile="${INV_PATCH_DIR}/${SUB_MODULE}/series"
        exec < ${seriesfile}

        while read line
        do
            content=$(echo $line | awk /#/'{print $1}')
            if [ "$content" = "#" ]; then
                continue
            fi

            content=$(echo $line | awk /CodeBase/'{print $1}')
            if [ "$content" = "[CodeBase]" ]; then
                content=$(echo $line | awk '{print $NF}')
                continue
            fi

            content=$(echo $line | awk /order/'{print $1}')
            if [ "$content" = "[order]" ]; then
                continue
            fi

            git am --whitespace=nowarn ${INV_PATCH_DIR}/${SUB_MODULE}/$line
            echo $line

        done
        echo
    else
        cd src/$SUB_MODULE
        echo "=== apply patch to ${SUB_MODULE} ==="
        CURRENT_HEAD="$(git rev-parse HEAD)"
        seriesfile="${INV_PATCH_DIR}/${SUB_MODULE}/series"
        exec < ${seriesfile}

        while read line
        do
            content=$(echo $line | awk /#/'{print $1}')
            if [ "$content" = "#" ]; then
                continue
            fi

            content=$(echo $line | awk /CodeBase/'{print $1}')
            if [ "$content" = "[CodeBase]" ]; then
                TARGET_VERSION=$(echo $line | awk '{print $NF}')
                if [ "$CURRENT_HEAD" != "$TARGET_VERSION" ]; then
                    echo "error --> $SUB_MODULE:Target version $TARGET_VERSION not match current $CURRENT_HEAD"
                    break
                fi
                continue
            fi

            content=$(echo $line | awk /order/'{print $1}')
            if [ "$content" = "[order]" ]; then
                continue
            fi

            git am --whitespace=nowarn ${INV_PATCH_DIR}/${SUB_MODULE}/$line

        done
        echo
    fi

   cd $BASE_PATH
   echo "already executed make init and patched files" > ${INV_PATCH_DIR}/already_patched
done
