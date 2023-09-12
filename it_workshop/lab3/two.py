import boto3

ec2 = boto3.resource('ec2')

instances = ec2.create_instances(
    ImageId='ami-0f5ee92e2d63afc18',  # replace with the actual ami-id
    InstanceType='t2.nano',
    MinCount=2,
    MaxCount=2,
)
for i in instances:
    print("Instance created:", i.id)
