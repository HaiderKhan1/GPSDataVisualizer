'use strict'

// C library API
const ffi = require('ffi-napi');
const mysql = require('mysql2/promise');

// Express App (Routes)
const express = require("express");
const app     = express();
const path    = require("path");
const fileUpload = require('express-fileupload');

app.use(fileUpload());
app.use(express.static(path.join(__dirname+'/uploads')));

// Minimization
const fs = require('fs');
const JavaScriptObfuscator = require('javascript-obfuscator');
const { json } = require('express');

// Important, pass in port as in `npm run dev 1234`, do not change
const portNum = process.argv[2];

// Send HTML at root, do not change
app.get('/',function(req,res){
  res.sendFile(path.join(__dirname+'/public/index.html'));
});

// Send Style, do not change
app.get('/style.css',function(req,res){
  //Feel free to change the contents of style.css to prettify your Web app
  res.sendFile(path.join(__dirname+'/public/style.css'));
});

// Send obfuscated JS, do not change
app.get('/index.js',function(req,res){
  fs.readFile(path.join(__dirname+'/public/index.js'), 'utf8', function(err, contents) {
    const minimizedContents = JavaScriptObfuscator.obfuscate(contents, {compact: true, controlFlowFlattening: true});
    res.contentType('application/javascript');
    res.send(minimizedContents._obfuscatedCode);
  });
});

//Respond to POST requests that upload files to uploads/ directory
app.post('/upload', function(req, res) {
  if(!req.files) {
    return res.status(400).send('No files were uploaded.');
  }
 
  var uploadFile = req.files.uploadFile;
 
  // Use the mv() method to place the file somewhere on your server
  uploadFile.mv('uploads/' + uploadFile.name, function(err) {
    if(err) {
      return res.status(500).send(err);
    }

    res.redirect('/');
  });
});

//Respond to GET requests for files in the uploads/ directory
app.get('/uploads/:name', function(req , res){
  fs.stat('uploads/' + req.params.name, function(err, stat) {
    if(err == null) {
      res.sendFile(path.join(__dirname+'/uploads/' + req.params.name));
    } else {
      console.log('Error in file downloading route: '+err);
      res.send('');
    }
  });
});

//******************** Your code goes here ******************** 

//Sample endpoint
app.get('/initFileCheck', function(req, res){
  let retStr = 0;

  const length = fs.readdirSync('./uploads').length;

  if(length > 0) {
    retStr = 1;
  }
  
  res.send(
    {
      state: retStr
    }
  );

});


app.listen(portNum);
console.log('Running app at localhost: ' + portNum);


let cLib = ffi.Library('./libgpxparser', {
  'GPXFILEtoJSON': ['string', ['string', 'string']], 'covertJSONtoGPX':['int', ['string','string','string']],
  'getRoutesinJSON':['string', ['string', 'string', 'float', 'float', 'float','float', 'float']],
  'getTracksinJSON':['string', ['string', 'string', 'float', 'float', 'float','float', 'float']],
  'addRte':['int', ['string','string','string']], 'addWpt':['int', ['string','string','string']],
  'validateFile':['int', ['string', 'string']]
});


app.get('/fileLogPanel', function(req, res) {
  let GPXdocs = [];
  let uploads = fs.readdirSync('./uploads');
  let counter = 0;
  for (let i=0;i<uploads.length;i++) {
    let jsonStr = cLib.GPXFILEtoJSON('./uploads/' + uploads[i], 'gpx.xsd');
    jsonStr = jsonStr.replace(/\n/g, "\\n").replace(/\r/g, "\\r").replace(/\t/g, "\\t");
    //console.log(jsonStr);
    if (jsonStr === null) {
      alert(uploads[i] + "failed to validate");
      continue;
    }

    if (jsonStr !== '{}') {
      var obj = JSON.parse(jsonStr);
      obj['file'] = uploads[i];
      GPXdocs[counter] = JSON.stringify(obj);
      counter++;
    }
  }


  
  //console.log(jsonArr);
  res.send({gpxdocs:GPXdocs, fileNum: counter});

});

app.get('/writeFile', function(req, res) {
  console.log(req.query.fileName);
  let gpxObj = {"version":req.query.version, "creator":req.query.creator};
  let gpxStr = JSON.stringify(gpxObj);
  let state = cLib.covertJSONtoGPX(gpxStr,'./uploads/' + req.query.fileName, 'gpx.xsd');
  //console.log(gpxStr);
  var bool = false;
  if (state === 1) {
    bool = true;
  }

  res.send(bool);
}); 

app.get('/getFileData', function(req, res) {
  
  let jsonStr = cLib.GPXFILEtoJSON('./uploads/' + req.query.fileName, 'gpx.xsd');
  jsonStr = jsonStr.replace(/\n/g, "\\n").replace(/\r/g, "\\r").replace(/\t/g, "\\t");
  if (jsonStr === null) {
    alert( req.query.fileName + "failed to validate");  
  }

  if (jsonStr !== '{}') {
    var obj = JSON.parse(jsonStr);
    obj['file'] = req.query.fileName;
    var retJSON= JSON.stringify(obj);

  }


 // console.log(retJSON);
  res.send(retJSON);

});

app.get('/validateFile', function(req, res) {
  let uploads = fs.readdirSync('./uploads');
  let bool = uploads.includes(req.query.fileName);
  res.send({state: bool});

});


app.get('/filesonServer', function(req, res) {
  let uploads = fs.readdirSync('./uploads');
  let counter = 0;
  let ret = [];
  
    for (let i=0;i<uploads.length;i++) {
      let jsonStr = cLib.GPXFILEtoJSON('./uploads/' + uploads[i], 'gpx.xsd');
      jsonStr = jsonStr.replace(/\n/g, "\\n").replace(/\r/g, "\\r").replace(/\t/g, "\\t");
      //console.log(jsonStr);
      if (jsonStr === null) {
        continue;
      }

      if (jsonStr !== '{}') {
        ret[counter] = uploads[i];
        counter++;
      }
    }
  
  
  res.send({
    files : ret, 
    filesNum: counter
  });

});


app.get('/getPathsBetween', function(req, res){

  var routes = [];
  var tracks = [];
  let uploads = fs.readdirSync('./uploads');
  let rC = 0;
  let tC = 0;
  for (let i=0;i<uploads.length;i++) {
    let route = cLib.getRoutesinJSON('./uploads/' + uploads[i], 'gpx.xsd', req.query.sLat, req.query.sLon, req.query.dLat, req.query.dLon, req.query.delta);
    let track = cLib.getTracksinJSON('./uploads/' + uploads[i], 'gpx.xsd', req.query.sLat, req.query.sLon, req.query.dLat, req.query.dLon, req.query.delta);

    if (route !== "0") {
      routes[rC++] = route;
    }
   
    if (track !== "0") {
      tracks[tC++] = track;
    }
    
  }
  
  res.send(
    {
     routesIn:routes,
     tracksIn:tracks
    }
  );

});

app.get('/addRoute', function(req, res){
  let rteObj = {"name":req.query.name};
  let state = cLib.addRte('./uploads/'+req.query.file, 'gpx.xsd', JSON.stringify(rteObj));
  console.log(state);
  let state2;

  for (let i = 0; i < req.query.length; i++) {
    state2 = cLib.addWpt('./uploads/'+req.query.file, 'gpx.xsd', JSON.stringify(req.query.wpts[i]));
  }
  

  res.send(
    {
      rte:state,
      wpt:state2
    }
  );

});

//// A4 Functions //////
var dbConf;

app.get('/login', async function(req, res) {
  
  dbConf = {
    host     : 'dursley.socs.uoguelph.ca',
    user     : req.query.username,
    password : req.query.password,
    database : req.query.databasename
  };

  let fileTable = "CREATE TABLE IF NOT EXISTS FILE (gpx_id INT NOT NULL AUTO_INCREMENT PRIMARY KEY, file_name VARCHAR(60) NOT NULL, ver DECIMAL(2,1) NOT NULL, creator VARCHAR(256) NOT NULL);";
  let rteTable = "CREATE TABLE IF NOT EXISTS ROUTE (route_id INT AUTO_INCREMENT PRIMARY KEY, route_name VARCHAR(256), route_len FLOAT(15,7) NOT NULL, gpx_id INT NOT NULL, FOREIGN KEY (gpx_id) REFERENCES FILE (gpx_id) ON DELETE CASCADE);";
  let pointTable = "CREATE TABLE IF NOT EXISTS POINT (point_id INT AUTO_INCREMENT PRIMARY KEY, point_index INT NOT NULL, latitude DECIMAL(11,7) NOT NULL, longitude DECIMAL(11,7) NOT NULL, point_name VARCHAR(256), route_id INT NOT NULL, FOREIGN KEY (gpx_id) REFERENCES FILE (gpx_id) ON DELETE CASCADE);";


  let state = 0;
  let connection;

  try {
    connection = await mysql.createConnection(dbConf);
    await connection.execute(fileTable);
    await connection.execute(rteTable);
    await connection.execute(pointTable);
    state = 1;
  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

  res.send(
    {
      state:state
    }
  );

});

app.get('/databaseStatus', async function(req, res) {
  
  let connection;
  let numFiles;
  let numRoutes;
  let numPoints;
  let ret = [];

  try {
    connection = await mysql.createConnection(dbConf);
    numFiles = await connection.execute("SELECT COUNT(*) FROM FILE;");
    numRoutes = await connection.execute("SELECT COUNT(*) FROM ROUTE;");
    numPoints = await connection.execute("SELECT COUNT(*) FROM POINT;");

  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

  ret.push(numFiles[0][0]["COUNT(*)"]);
  ret.push(numRoutes[0][0]["COUNT(*)"]);
  ret.push(numPoints[0][0]["COUNT(*)"]);
  res.send({ret});

});

app.get('/clearDatabase', async function(req, res) {
  
  let connection;
  let state = 0;

  try {
    connection = await mysql.createConnection(dbConf);
    await connection.execute("DELETE from FILE");
    state = 1;

  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

  //console.log(state);
  res.send({state});

});



app.get('/routePanel', async function(req, res) {
  
  let connection;
  
  try {
    connection = await mysql.createConnection(dbConf);
    const [rows, cols] = await connection.execute('SELECT * FROM ROUTE ORDER BY route_name;');
    res.send({records:rows, feilds:cols});

  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

});

app.get('/qFilePanel', async function(req, res) {
  
  let connection;
  
  try {
    connection = await mysql.createConnection(dbConf);
    const [rows, cols] = await connection.execute('SELECT * FROM FILE ORDER BY file_name;');
    res.send({records:rows, feilds:cols});

  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

});

app.get('/q3', async function(req, res) {
  
  let connection;
  
  try {
    connection = await mysql.createConnection(dbConf);
    //const [row2, col1] = await connection.execute('SELECT route_id from ROUTE where route_name = \"'+req.query.rteName+'\" ');
    const [rows, cols] = await connection.execute('SELECT * FROM POINT WHERE route_id = \"'+req.query.rte_id+'\" ORDER BY point_index;');
    res.send({records:rows, feilds:cols});

  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

});


app.get('/q5', async function(req, res) {
  let connection;
  try {
    connection = await mysql.createConnection(dbConf);
    const [row2, col1] = await connection.execute('SELECT gpx_id from FILE where file_name = \"'+req.query.fileName+'\" ');
    if (req.query.qType === "longest") {
      if (req.query.nameSort === true) {
        const [rows, cols] = await connection.execute('SELECT * FROM ROUTE WHERE gpx_id = \"'+row2[0].gpx_id+'\" ORDER BY route_len DESC, route_name LIMIT '+req.query.limit+';');   
        res.send ({records:rows});
      } else {
        const [rows, cols] = await connection.execute('SELECT * FROM ROUTE WHERE gpx_id = \"'+row2[0].gpx_id+'\" ORDER BY route_len DESC LIMIT '+req.query.limit+';');   
        res.send ({records:rows});
      }
   
    } else if (req.query.qType === "shortest") {
      if (req.query.nameSort === true) {
        const [rows, cols] = await connection.execute('SELECT * FROM ROUTE WHERE gpx_id = \"'+row2[0].gpx_id+'\" ORDER BY route_len ASC, route_name LIMIT '+req.query.limit+';');   
        res.send ({records:rows});
      } else {
        const [rows, cols] = await connection.execute('SELECT * FROM ROUTE WHERE gpx_id = \"'+row2[0].gpx_id+'\" ORDER BY route_len ASC LIMIT '+req.query.limit+';');   
        res.send ({records:rows});
      }
     
    }

  } catch (e) {
    console.log("Query error: " + e);
  } finally {
    if (connection && connection.end) connection.end();
  }

});
