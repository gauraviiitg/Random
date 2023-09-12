#!/bin/bash
yum update -y
yum install httpd -y
aws s3 sync s3://lab2 /var/www/html/
service httpd start
chkconfig httpd on
