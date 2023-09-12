import boto3

ec2 = boto3.resource('ec2')

running_instances = ec2.instances.filter(
    Filters=[{'Name': 'instance-state-name', 'Values': ['running']}]
)
for instance in running_instances:
    print(f"Instance {instance.id} of type {instance.instance_type} is running.")
