eb = boto3.client('elasticbeanstalk')

app_name = 'MyPersonalWebsite'
eb.create_application(ApplicationName=app_name)

env_name = 'lab6-env'
version_label = 'v1'

eb.create_application_version(
    ApplicationName=app_name,
    VersionLabel=version_label,
    SourceBundle={
        'S3Bucket': 'lab2',
        'S3Key': 'myapp.zip'
    }
)

eb.create_environment(
    ApplicationName=app_name,
    EnvironmentName=env_name,
    VersionLabel=version_label,
    SolutionStackName='64bit Amazon Linux 2023.03 v2.9.0 running Python 3.11'
)
