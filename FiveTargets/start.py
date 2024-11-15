from urllib import response
import serial # type: ignore
import time
import json
import requests # type: ignore

# The API endpoint
url = "http://midtskips.no/uiaod/receiver.php"

def read_from_com_port(com_port, baud_rate, output_file, current_name):
        
    # Open the serial port
    with serial.Serial(com_port, baud_rate, timeout=1) as ser:    
        
        print(f"Listening on {com_port} at {baud_rate} baud...")
        
        while True:
            # Read a line from the COM port
            line = ser.readline().decode('ascii').strip()
            
            if line:
                print(f"Received: {line}")
                # Check if the line contains the time result
                if "Time used: " in line:
                    # Extract the time result, but ignoring the hour element
                    time_result = line.split(": 00:")[1]
                    time_result = time_result.strip(" s")
                    # Data to be sent
                    data = {}
                    data['Name'] = current_name
                    data['Time'] = time_result
                    json_data = json.dumps(data)
                    print(json_data)
                    
                    # A POST request to the API
                    response = requests.post(url, json=data)

                    # Print the response
                    print(response.text)

                    # Write the name and time result to the file
                    with open(output_file, 'a') as file:
                        file.write(f"{current_name}, {time_result}\n")
                        file.flush()  # Ensure data is written to the file immediately
                    print(f"Logged: {current_name}, {time_result}")
                    return
            # Optional: Add a small delay to avoid overwhelming the CPU
            time.sleep(0.1)

if __name__ == "__main__":
    # Configure your COM port and baud rate
    COM_PORT = 'COM14'  # Change this to your COM port
    BAUD_RATE = 9600    # Change this to your baud rate
    #OUTPUT_FILE = 'results.txt'  # Change this to your desired output file
    OUTPUT_FILE = "C:/xampp/htdocs/midtskips.no/uiaod/results.txt"
    #print("Enter names to link with time results. Type 'exit no' to stop.")
    portNumber = input("Enter com-port to use [1,2,3,..,n]: ").strip().lower()
    if (portNumber.isdigit()):
        COM_PORT = 'COM'+portNumber
    else:
        print("COM",portNumber," is not valid port number! Exiting.")
        exit()
    
    print("Will use", COM_PORT)

    while True:
        # Prompt the user for a name
        current_name = input("Enter a name (or 'exit' to finish): ")
        
        if current_name.lower() == "exit":
            print("Exiting the program.")
            break
        else:
            # Start reading from the COM port for the current name
            try:
                read_from_com_port(COM_PORT, BAUD_RATE, OUTPUT_FILE, current_name)
            except KeyboardInterrupt:
                print("Program terminated by user.")
            except Exception as e:
                print(f"An error occurred: {e}")
