#!/bin/bash
## This script is to retrieve and import the docker-base image from
## local folder where the image is built on the local machine
##
## USAGE:
##   ./get_docker-base.sh

set -x -e

. ./functions.sh

TARGET_PATH=$(sed -n 's/TARGET_PATH\s*=\s*//p' slave.mk)

## [SuppressMessage("Microsoft.Security", "CS002:SecretInNextLine", Justification="Read-only link of Azure Blob storage with shared access signature (SAS)")]
BASE_URL="https://sonicstorage.blob.core.windows.net/packages/docker-base.ea507753d98b0769e2a15be13003331f8ad38d1c15b40a683e05fc53b1463b10.gz?sv=2015-04-05&sr=b&sig=YNN6eYVMEFndUaiHIRnqcZFdDZwIG%2BaAuVj0IoyDWPw%3D&se=2026-10-27T20%3A46%3A18Z&sp=r"

base_image_name=docker-base
docker_try_rmi $base_image_name
mkdir -p $TARGET_PATH
wget --no-use-server-timestamps -O $TARGET_PATH/$base_image_name.gz "$BASE_URL"
docker load < $TARGET_PATH/$base_image_name.gz
