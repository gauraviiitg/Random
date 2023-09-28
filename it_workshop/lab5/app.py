from flask import Flask, request, jsonify
import pymysql

app = Flask(__name__)

# Configure database
db_host = "lab5.c2na0up97cuj.ap-south-1.rds.amazonaws.com"
db_user = "admin"
db_password = "password"
db_name = "FeedbackDB"

@app.route('/submit-feedback', methods=['POST'])
def submit_feedback():
    name = request.form.get('name')
    feedback = request.form.get('feedback')

    # Create a database connection
    connection = pymysql.connect(host=db_host,
                                 user=db_user,
                                 password=db_password,
                                 database=db_name)

    try:
        with connection.cursor() as cursor:
            sql = "INSERT INTO feedback (name, feedback) VALUES (%s, %s)"
            cursor.execute(sql, (name, feedback))
            connection.commit()

        return jsonify({"message": "Feedback submitted successfully!"}), 200
    except Exception as e:
        return jsonify({"error": str(e)}), 500
    finally:
        connection.close()

if __name__ == '__main__':
    app.run(port=5000)
