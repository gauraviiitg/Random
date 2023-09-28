import boto3
import pymysql

# Initialize the Boto3 RDS client
rds = boto3.client('rds')

# Specify your RDS instance details
db_instance_identifier = 'lab5'
db_username = 'admin'
db_password = '$inghadi3'
db_name = 'FeedbackDB'

try:
    # Create a database connection using pymysql
    conn = pymysql.connect(
        host=f"lab5.cd7lwzfxcq03.ap-south-1.rds.amazonaws.com",
        user=db_username,
        password=db_password,
        database=db_name,
        port=3306,  # Default MySQL port
        connect_timeout=10,
    )

    cursor = conn.cursor()

    # Create your tables and define their schemas
    create_table_query = """
    CREATE TABLE IF NOT EXISTS feedback (
        id INT AUTO_INCREMENT PRIMARY KEY,
        name VARCHAR(255) NOT NULL,
        email VARCHAR(255) NOT NULL,
        message TEXT NOT NULL,
        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
    )
    """

    # Execute the table creation query
    cursor.execute(create_table_query)

    # Commit the changes
    conn.commit()

    print("Database and table created successfully.")
except pymysql.MySQLError as e:
    print(f"Error: {e}")
finally:
    if 'conn' in locals() and conn.open:
        cursor.close()
        conn.close()
