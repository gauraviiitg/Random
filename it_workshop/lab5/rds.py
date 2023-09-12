from flask import Flask, request, jsonify
import mysql.connector

app = Flask(__name__)

# Database connection configuration
db_config = {
    "host": "mydbinstance.c2na0up97cuj.ap-south-1.rds.amazonaws.com",
    "user": "gaurav",
    "password": "monugaurav",
    "database": "mydatabase",
}

@app.route("/api/submit-feedback", methods=["POST"])
def submit_feedback():
    try:
        # Get feedback from the form submission
        feedback = request.form.get("feedback")

        # Connect to the database
        conn = mysql.connector.connect(**db_config)
        cursor = conn.cursor()

        # Insert feedback into the database
        insert_query = "INSERT INTO feedback (message) VALUES (%s)"
        cursor.execute(insert_query, (feedback,))
        conn.commit()

        # Close database connection
        cursor.close()
        conn.close()

        return jsonify({"message": "Feedback submitted successfully!"}), 201

    except Exception as e:
        return jsonify({"error": str(e)}), 500

if __name__ == "__main__":
    app.run()
