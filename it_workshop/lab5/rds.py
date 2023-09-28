import boto3

def create_rds_instance(instance_name, db_username, db_password, security_group_id):
    client = boto3.client('rds', region_name='ap-south-1')

    response = client.create_db_instance(
        DBName='FeedbackDB',
        DBInstanceIdentifier=instance_name,
        AllocatedStorage=20,
        DBInstanceClass='db.t2.micro',
        Engine='mysql',
        MasterUsername=db_username,
        MasterUserPassword=db_password,
        VpcSecurityGroupIds=[
            security_group_id,
        ],
        PubliclyAccessible=True,
    )

    print(f"RDS instance {instance_name} is being created. This might take some time.")
    waiter = client.get_waiter('db_instance_available')
    waiter.wait(DBInstanceIdentifier=instance_name)
    print(f"RDS instance {instance_name} has been created!")

# Replace these placeholders with actual values before running the script
INSTANCE_NAME = "lab5"
DB_USERNAME = "admin"
DB_PASSWORD = "$inghadi3"
SECURITY_GROUP_ID = "sg-003c7862d8429fa1f"

create_rds_instance(INSTANCE_NAME, DB_USERNAME, DB_PASSWORD, SECURITY_GROUP_ID)

