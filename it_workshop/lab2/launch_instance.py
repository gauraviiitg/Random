import boto3
import time

# AWS credentials and region
aws_access_key = 'AKIASXOABBVHI63YRJEO'
aws_secret_key = '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'
aws_region = 'ap-south-1'

# Name of the S3 bucket and website files
# s3_buck = 'vpbucket85'
# website_files = ['index.html', 'index.css', 'ml.jpeg', 'mountain.jpeg', 'skills.jpeg', 'favicon.ico', 'dsa.jpeg',
#                  'dpnew-modified.png', 'dev.jpg', 'cloud.png']


# User data script to install Apache and copy website files
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

# Create an EC2 client
ec2_client = boto3.client('ec2', aws_access_key_id=aws_access_key, aws_secret_access_key=aws_secret_key,
                          region_name=aws_region)

# Launch an instance
instance = ec2_client.run_instances(
    ImageId='ami-0ded8326293d3201b',
    InstanceType='t2.micro',
    KeyName='lab2',
    IamInstanceProfile={
        'Arn': 'arn:aws:iam::187770670414:instance-profile/lab2'
    },
    SecurityGroups=['lab2'],
    UserData=startup_script,
    MaxCount=1,
    MinCount=1
)

instance_id = instance['Instances'][0]['InstanceId']

# Wait for the instance to be running
while True:
    instance_info = ec2_client.describe_instances(InstanceIds=[instance_id])
    instance_state = instance_info['Reservations'][0]['Instances'][0]['State']['Name']
    if instance_state == 'running':
        public_dns = instance_info['Reservations'][0]['Instances'][0]['PublicDnsName']
        break
    time.sleep(5)

print(f"Instance {instance_id} is running. Public DNS: {public_dns}")

# Open the instance's public DNS in a browser to verify the static website
print(f"Open this URL in your browser: http://{public_dns}/")