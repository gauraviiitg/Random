from flask import Flask, render_template, request, redirect
import json

app = Flask(__name__)

@app.route('/', methods=['GET', 'POST'])
def index():
    # If form data is posted
    if request.method == 'POST':
        data = {
            "title": request.form['title'],
            "introduction": request.form['introduction'],
            "about_me": request.form['about_me'],
            "my_work": request.form['my_work'],
            "contact_info": request.form['contact_info'],
            "footer": request.form['footer']
        }
        with open('data.json', 'w') as file:
            json.dump(data, file)
        
        return redirect('/')  # Redirect back to homepage after saving

    # Otherwise, just render the page
    with open('data.json', 'r') as file:
        data = json.load(file)
    return render_template('index.html', **data)

if __name__ == '__main__':
    app.run(debug=True)
