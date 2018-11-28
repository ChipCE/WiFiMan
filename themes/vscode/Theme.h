#ifndef __THEME_H
#define __THEME_H
#include <Arduino.h>

//vscode color scheme
const char custom_css[] PROGMEM = R"(
.c{
    text-align: center;
}
div{
    padding: 5px;
    font-size: 1em;
}
input{
    width: 95%;
    margin-bottom: 5px;
    border-radius: 4px;
    padding: 5px;
    font-size: 1em;
}
body {
    height: 100%;
    text-align: center;
    font-family: verdana;
    background-image :none;
    background-color:#383838;
}
button {
    line-height: 2.4rem;
    font-size: 1.2rem;
    width: 100%;
    background-image:none;
    background-color:#252525;
    color:#ffffff;
    border-width: 0px 3px 3px 0px;
    border-color: #519aba;
    border-style: solid;
}
#header {
    height: 120px;
    background-image:none;
    background-color:#1e1e1e;
    color:#519aba;
    text-align: left;
}
#footer {
    background-image:none;
     background-color:#1e1e1e;
    color:#519aba;
}
#body {
    text-align: left;
    display: inline-block;
    min-width: 260px;
    min-height: calc(100vh - 240px);
    color:#ffffff;
}
h2{
    margin-top: 4px;
    margin-left: 10px;
}
p{
    margin-top: 2px;
    margin-bottom: 0px;
    margin-left: 10px;
})";
#endif