var requestpending;
var killswitch = 0;
var datacounter = 0;
var timer;
var xmlhttp;

var url= "/temp.shtml";

function xmlrequeststatechange() 
  { 
  if (xmlhttp.readyState==4 && xmlhttp.status==200) 
    { 
	try {
	  var s = xmlhttp.responseText;
      document.getElementById("displayCelsius").innerHTML=s; 
    }
    catch(err) { }
    requestpending=false;
    } 
  } 
  
function xmlrequesttimeout() {
  requestpending=false;
}  
 
function dattding() 
{ 
  if (requestpending) {
    killswitch++;
	if (killswitch<20) {
      return;
	} else {
      xmlhttp.abort();
    }
  }
  
  xmlhttp=new XMLHttpRequest(); 
 
  xmlhttp.onreadystatechange=xmlrequeststatechange;
  xmlhttp.ontimeout=xmlrequesttimeout;
  xmlhttp.open("GET",url,true); 
  xmlhttp.timeout = 10000;
  xmlhttp.send(); 

  requestpending=true;
} 

function bstopp() {
  clearInterval(timer);
}

function bstart() {
  clearInterval(timer);
  timer=setInterval(dattding, 5000);
}


window.onload=bstart;
// end