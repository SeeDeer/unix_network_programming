#!/bin/bash

aliyun_ecs_ip=139.224.135.102

echo "upload local file to aliyun_ecs."

read -p "please file or directory of name:"  local_file
scp $local_file root@$aliyun_ecs_ip:/root/