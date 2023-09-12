import boto3

# Initialize EC2 resource and client
ec2 = boto3.resource('ec2')
ec2_client = boto3.client('ec2')

# User Data Script to setup HTTP server and fetch content from S3
startup_script = """#!/bin/bash
yum install -y httpd
systemctl start httpd
usermod -a -G apache ec2-user
chown -R ec2-user:apache /var/www
chmod 2775 /var/www
mkdir /var/www/html/ 
aws s3 sync s3://lab2 /var/www/html/
systemctl restart httpd
"""

# Fetch the ID of the security group named 'lab2'
security_groups = ec2_client.describe_security_groups(GroupNames=['lab2'])
security_group_id = security_groups['SecurityGroups'][0]['GroupId']

# Create EC2 instance with specified configurations
instance = ec2.create_instances(
    ImageId='ami-0da59f1af71ea4ad2',  # replace with the actual ami-id
    InstanceType='t2.micro',
    MinCount=1,
    MaxCount=1,
    UserData=startup_script,
    SecurityGroupIds=[security_group_id]
)

print("HTTP Server instance created:", instance[0].id)
