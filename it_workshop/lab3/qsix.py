import boto3

ec2 = boto3.client('ec2')

response = ec2.describe_instances(
    Filters=[{'Name': 'instance-state-name', 'Values': ['running']}]
)

instance_ids = []
for reservation in response['Reservations']:
    for instance in reservation['Instances']:
        instance_ids.append(instance['InstanceId'])

if instance_ids:
    ec2.stop_instances(InstanceIds=instance_ids)
    print(f"Stopped instances: {', '.join(instance_ids)}")
