@echo off
:: Update and install Apache
choco install -y apache-httpd
:: Sync website files from S3
aws s3 sync s3://lab2 C:\Apache24\htdocs\
:: Start Apache
httpd -k start
