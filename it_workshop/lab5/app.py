from flask import Flask, request, render_template
import mysql.connector

app = Flask(__name__)

# RDS database parameters
db_host = 'mydbinstance.c2na0up97cuj.ap-south-1.rds.amazonaws.com'  # Replace with your RDS endpoint
db_user = 'gaurav'
db_password = 'monugaurav'
db_name = 'mydatabase'

@app.route('/')
def index():
    return render_template('index.html')

@app.route('/submit-feedback', methods=['POST'])
def submit_feedback():
    feedback = request.form['feedback']

    try:
        # Create a connection to the RDS database
        connection = mysql.connector.connect(
            host=db_host,
            user=db_user,
            password=db_password,
            database=db_name
        )

        if connection.is_connected():
            cursor = connection.cursor()
            # Insert the feedback into the database
            cursor.execute("INSERT INTO comments (comment) VALUES (%s)", (feedback,))
            connection.commit()
            cursor.close()
            return "Feedback submitted successfully!"
    
    except mysql.connector.Error as e:
        print("Error:", e)
        return "An error occurred while submitting feedback."

    finally:
        if connection.is_connected():
            connection.close()
            print("Connection closed")

if __name__ == '__main__':
    app.run(debug=True)
