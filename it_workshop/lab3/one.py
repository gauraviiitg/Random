
import boto3

ec2 = boto3.resource('ec2')

instance = ec2.create_instances(
    ImageId='ami-0da59f1af71ea4ad2',  # replace with the actual ami-id
    InstanceType='t2.micro',
    MinCount=1,
    MaxCount=1,
)
print("Instance created:", instance[0].id)
