import boto3

cf = boto3.client('cloudfront')

eb_environment_url = "http://lab6-env.eba-bnvxjmqi.ap-south-1.elasticbeanstalk.com/"

response = cf.create_distribution(
    DistributionConfig={
        'CallerReference': 'for_lab6',  
        'Origins': {
            'Quantity': 1,
            'Items': [
                {
                    'Id': 'ElasticBeanstalkOrigin',
                    'DomainName': eb_environment_url,
                    'OriginPath': '',
                    'CustomOriginConfig': {
                        'HTTPPort': 80,
                        'HTTPSPort': 443,
                        'OriginProtocolPolicy': 'http-only',
                        'OriginSslProtocols': {
                            'Quantity': 1,
                            'Items': ['TLSv1.2']
                        }
                    }
                }
            ]
        },
        'DefaultCacheBehavior': {
            'TargetOriginId': 'ElasticBeanstalkOrigin',
            'ForwardedValues': {
                'QueryString': True,
                'Cookies': {
                    'Forward': 'none',
                }
            },
            'TrustedSigners': {
                'Enabled': False,
                'Quantity': 0
            },
            'ViewerProtocolPolicy': 'redirect-to-https',
            'AllowedMethods': {
                'Quantity': 3,
                'Items': ['GET', 'HEAD', 'OPTIONS'],
                'CachedMethods': {
                    'Quantity': 2,
                    'Items': ['GET', 'HEAD']
                }
            },
        },
        'Comment': 'CloudFront Distribution for Elastic Beanstalk',
        'PriceClass': 'PriceClass_100',
        'Enabled': True,
    }
)

print(response['Distribution']['DomainName'])
