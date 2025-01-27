# weather-cli
A simple cli weather app developed using C language.
## Dependencies
Run the following command:
```
sudo apt-get install libcurl4-openssl-dev
sudo apt-get install libcjson-dev
```
## How to run
Make sure to have gcc installed. Then compile it as such:
```
gcc app.c -o app -lcurl -lcjson
```
Then, run the file. The example below display the weather data for Tokyo.
```
./app "Tokyo"
```
