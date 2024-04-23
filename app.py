from flask import Flask, jsonify, request, make_response, render_template
import datetime
import json

app = Flask(__name__)

def get_current_date():
    now = datetime.datetime.now()
    day_of_week = now.strftime("%a")
    month = now.strftime("%b")
    current_date = "%s, %s %s %s %s.%s" % (
        day_of_week,
        now.day,
        month,
        now.year,
        now.strftime("%H:%M:%S"),
        now.microsecond // 10000,
    )
    print("[INFO] using current date: %s" % current_date)

    return current_date


@app.route("/")
def index():
    response = make_response("Data has been received")
    response.headers["Cache-Control"] = "public, max-age=31536000"

    return render_template("index.html", data=data)

@app.route("/data")
def get_data():
    response = make_response("Data has been received")

    return render_template("data.html", data=data)


@app.route("/set_date")
def set_date():
    # sets the date on each lane/arduino
    # ie Fri, 16 Feb 2024 22:54:33 GMT
    current_date = get_current_date()
    response = make_response("Data has been received")
    response.headers["startTime"] = current_date

    return response


@app.route("/start", methods=["GET"])
def start():
    # The official will need to use a button, or starter which will
    # trigger this URI and set the startTime for the URI race_start_time
    # This will also reset/zero out the scoreboard
    global data
    global final_results
    global race_start_time

    data = []
    final_results = {}

    current_date = get_current_date()
    race_start_time = current_date
    print("[INFO] new race startTime: %s" % current_date)

    response = make_response("Data has been received")
    response.headers["startTime"] = current_date

    return response


@app.route("/race_start_time")
def race_start_time():
    # each lane grabs this w/button press, then will calc elapsedTime
    # post it to /results

    response = make_response("Data has been received")
    response.headers["startTime"] = race_start_time

    return response


def heat_results(results={}):
    global data
    data = []

    for lane in results:
        if lane in final_results:
            final_results[lane] = results[lane]

    sorted_final_results = dict(
        sorted(
            final_results.items(), key=lambda item: tuple(map(int, item[1].split(":")))
        )
    )
    # get the length to display the last set of placements/lanes
    # in theory, placement could change and list gets long
    # so account for that
    length_results = len(sorted_final_results)

    for index, f in enumerate(sorted_final_results):
        placement = index + 1
        print("%s: %s, %s" % (f, placement, sorted_final_results[f]))
        data.append({"place": placement, "lane": f, "time": sorted_final_results[f]})

    # this is a global, so is picked up by index route
    # returns the last result per lane
    data = data[-length_results:]

    return final_results


@app.route("/results", methods=["GET", "POST"])
def results():
    data_b = request.get_data()
    data_str = data_b.decode("utf-8")
    data = json.loads(data_str)

    results = heat_results()
    for lane in data.keys():
        results[lane] = data[lane]
        results = heat_results(results)

    response = make_response("/results has been received.")
    response.headers["Refresh"] = "0; url=http://0.0.0.0:8000/data"

    return response


if __name__ == "__main__":
    global data
    global final_results
    global length_results
    length_results = 0
    final_results = {}
    data = []

    app.run(host="0.0.0.0", port=8000, debug=True)
