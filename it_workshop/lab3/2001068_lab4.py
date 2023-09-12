import boto3
import time

aws_access_key = 'AKIASXOABBVHI63YRJEO'
aws_secret_key = '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'
aws_region = 'ap-south-1'

#2001068 - Gaurav choudhary - lab 4

startup_script = """#!/bin/bash     # not much changed since question said to use script from part 2 for part 3
yum install -y httpd          
systemctl start httpd
usermod -a -G apache ec2-user
chown -R ec2-user:apache /var/www
chmod 2775 /var/www
mkdir /var/www/html/
aws s3 sync s3://lab2 /var/www/html/
systemctl restart httpd
"""
ec2_client = boto3.client('ec2', aws_access_key_id=aws_access_key, aws_secret_access_key=aws_secret_key, region_name=aws_region)
as_client = boto3.client('autoscaling', aws_access_key_id=aws_access_key, aws_secret_access_key=aws_secret_key, region_name=aws_region)
cw_client = boto3.client('cloudwatch', aws_access_key_id=aws_access_key, aws_secret_access_key=aws_secret_key, region_name=aws_region)

launch_config_name = "lab2_launch_config"

# Delete the existing launch configuration if it exists
try:
    as_client.delete_launch_configuration(LaunchConfigurationName=launch_config_name)
    print(f"Deleted existing launch configuration: {launch_config_name}")
except as_client.exceptions.ResourceInUseFault:
    print(f"Launch configuration {launch_config_name} does not exist. Creating a new one...")

# Create a new launch configuration
as_client.create_launch_configuration(
    LaunchConfigurationName=launch_config_name,
    ImageId='ami-0ded8326293d3201b',
    InstanceType='t2.micro',
    KeyName='lab2',
    IamInstanceProfile='arn:aws:iam::187770670414:instance-profile/lab2',
    SecurityGroups=['lab2'],
    UserData=startup_script
)

asg_name = "lab2_asg"
as_client.create_auto_scaling_group(
    AutoScalingGroupName=asg_name,
    LaunchConfigurationName=launch_config_name,
    MinSize=1, #this forces us to close the asg completely
    MaxSize=5,
    DesiredCapacity=1,
    AvailabilityZones=[aws_region+'a']
)

scale_up_policy = as_client.put_scaling_policy(
    AutoScalingGroupName=asg_name,
    PolicyName='ScaleUp',
    ScalingAdjustment=1, #to steadily increase the load (ex. site used in daylight)
    AdjustmentType='ChangeInCapacity'
)

scale_down_policy = as_client.put_scaling_policy(
    AutoScalingGroupName=asg_name,
    PolicyName='ScaleDown',
    ScalingAdjustment=-1,
    AdjustmentType='ChangeInCapacity'
)

cw_client.put_metric_alarm(
    AlarmName='ScaleUpAlarm',
    MetricName='CPUUtilization',
    Namespace='AWS/EC2',
    Statistic='Average',
    Period=300,
    Threshold=50,
    ComparisonOperator='GreaterThanThreshold',
    EvaluationPeriods=1,
    AlarmActions=[scale_up_policy['PolicyARN']],
    Dimensions=[{'Name': 'AutoScalingGroupName', 'Value': asg_name}]
)

cw_client.put_metric_alarm(
    AlarmName='ScaleDownAlarm',
    MetricName='CPUUtilization',
    Namespace='AWS/EC2',
    Statistic='Average',
    Period=300,
    Threshold=30,
    ComparisonOperator='LessThanThreshold',
    EvaluationPeriods=20, # can be loered if site is less visited
    AlarmActions=[scale_down_policy['PolicyARN']],
    Dimensions=[{'Name': 'AutoScalingGroupName', 'Value': asg_name}]
)

print(f"Auto Scaling Group {asg_name} is set up and ready!")