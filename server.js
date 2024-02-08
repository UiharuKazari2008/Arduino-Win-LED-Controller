const {SerialPort, ReadlineParser} = require('serialport');
const yargs = require('yargs/yargs')
const {hideBin} = require('yargs/helpers');
const sleep = (waitTimeInMs) => new Promise(resolve => setTimeout(resolve, waitTimeInMs));
const express = require('express');
const app = express();

const cliArgs = yargs(hideBin(process.argv))
    .option('port', {
        alias: 'p',
        type: 'string',
        description: 'Keychip Serial Port\nCOM5 is default port'
    })
    .argv

const port = new SerialPort({path: cliArgs.port || "COM1", baudRate: 9600});
const parser = port.pipe(new ReadlineParser({delimiter: '\n'}));
parser.on('data', (data) => {
    console.log(data);
});

function hexToRgb(hex) {
    hex = hex.replace(/^#/, '');
    const bigint = parseInt(hex, 16);
    const r = (bigint >> 16) & 255;
    const g = (bigint >> 8) & 255;
    const b = bigint & 255;
    return [r,g,b];
}

app.get('/led/:sec/state/:enable', (req, res) => {
    let result = {};
    result[req.params.sec] = { enable: !!(req.params.enable === "true"), mode: 1};
    if (req.query.brightness) {
        result[req.params.sec].brightness = parseInt(req.query.brightness);
    }
    port.write('' + JSON.stringify(result) + '\n');
    res.status(200).json(result);
})
app.get('/led/:sec/color', (req, res) => {
    let data = {
        mode: 0,
        enable: true
    };
    if (req.query.brightness) {
        data.brightness = parseInt(req.query.brightness);
    }
    if (req.query.start_color) {
        data.color = [
            hexToRgb(req.query.start_color)
        ];
        if (req.query.end_color) {
            data.color.push(hexToRgb(req.query.end_color));
        }
    }
    let result = {};
    result[req.params.sec] = data;
    port.write('' + JSON.stringify(result) + '\n');
    res.status(200).json(result);
})
app.get('/led/:sec/lgbt', (req, res) => {
    let result = {};
    result[req.params.sec] = { enable: true, mode: 1};
    if (req.query.brightness) {
        result[req.params.sec].brightness = parseInt(req.query.brightness);
    }
    port.write('' + JSON.stringify(result) + '\n');
    res.status(200).json(result);
})

port.on('open', async () => {
    app.listen(8733, () => {
        console.log(`LED Server on http://0.0.0.0:8733`);
    });
});
port.on('error', async (err) => {
    console.error(err);
});
port.on('close', async (err) => {
    console.error(err);
});
