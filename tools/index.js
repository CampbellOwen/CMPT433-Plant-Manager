/* This module is cli flasher that can be used to 
convert a folder into .bin formart and flash it to 
the esp's file system */
const cliArgs = require('command-line-args');
const cliUsage = require('command-line-usage');



let cliArgsDefintions = [
  {name: 'input-folder', alias: 'i', type: String, multiple: false },
  {name: 'flash-bin', alias: 'f', type: String, multiple: false }
];

const cliArgSections = [
  {
    header: 'Esp8266 flasher',
    content: 'A Simple cli tool to help you generate a bin file and flash to the [underline]{Esp8266}.'
  },
  {
    header: 'Options',
    optionList: [
      {
        name: 'input-folder',
        typeLabel: '[underline]{path to dir}',
        description: 'Path to the folder you want to convert to bin' 
      },
      {
        name: 'flash-bin',
        typeLabel: '[underline]{path to the file}',
        description: 'Path to the bin file to upload to board' 
      },
      {
        name: 'help',
        description: 'Print this usage guide.'
      }
    ]
  }
];


const options = cliArgs(cliArgsDefintions);
console.log(options);