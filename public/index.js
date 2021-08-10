// Put all onload AJAX calls here, and event listeners
jQuery(document).ready(function() {
    // On page-load AJAX Example
    jQuery.ajax({
        type: 'get',            //Request type
        dataType: 'json',       //Data type - we will use JSON for almost everything 
        url: '/initFileCheck',   //The server endpoint we are connecting to
        data: {},
        success: function (data) {
            //We write the object to the console to show that the request was successful
            console.log(data);
            populateFileLog(data.state);
            populateFileLister(data.state);

        },
        fail: function(error) {
            // Non-200 return, do something with error
            console.log(error); 
        }
    });

    function populateFileLister(data) {   
        if ( data == 0) {
            alert("No files in server, please upload the files");
        } else if (data == 1) {
            $.ajax({
                type: 'get',
                dataType: 'json',
                url: '/filesOnServer',
                data : {},
                success: function (data) {
                    //console.log(data.files);
                    for (let i = 0; i < data.files.length; i++) {
                        $('#FileSelector').append('<option id =' + data.files[i]+'>' + data.files[i]+' </option>');
                        $('#fsAdd').append('<option id =' + data.files[i]+'>' + data.files[i]+' </option>');
                    
                    }
                }, 
                fail: function(error) {
                    console.log(error); 
                }
    
            }); 
        }
    };



    $("#FileSelector").change(function() {
        var input = $(this).children("option:selected").val();

        if(input !== "Select File") {
            $.ajax({
                type:'get',
                dataType: 'json',
                url: '/getFileData',
                data: {
                    fileName:input
                },
                success: function(data) {
                    $('#viewPanel').append('<tr><th>Component</th><th>name</th><th>numPoints</th><th>Length</th><th>Loop</th></tr>');
                    
                    //console.log(data);
                    let routes = data.routes;
                   // console.log(data);
                    let rSize = routes.length;
                    if (rSize === 0) {
                        alert("No roots in this file");
                    } else {
                        for ( let i = 0; i < rSize; i++) {
                       
                            $('#viewPanel').append('<tr><td>Route</td><td>'+routes[i].name+' <button type="submit" id="route'+i+'">Show Data</button></td><td>'+routes[i].numPoints+'</td><td>'+routes[i].len+'</td><td>'+routes[i].loop+'</td></tr>');
                           // console.log(data.file);
                        }
    
                        $('#viewPanel').on('click', 'button[id^="route"]', function (e) {
                            
                            let id = this.id; 
                            let x = id.slice(5);
                            let len = routes[x].otherData.length;
                            
                            if (len === 0) {
                                alert("No other Data");
                            } else {
                                for (let i = 0; i < len; i++) {
                                    var str = routes[x].otherData[i].name + ":" + routes[x].otherData[i].value + " ";
                                }
        
                               alert(str);
                            }
                            
                        });
                    }
                  

                    //handle the tracks 
                    let tracks = data.tracks;
                    // console.log(data);
                     let tSize = tracks.length;
                     if (tSize === 0) {
                         alert("No tracks in this file");
                     } else {
                        for (let i = 0; i < tSize; i++) {
                            $('#viewPanel').append('<tr><td>Track</td><td>'+tracks[i].name+' <button type="submit" id="tracks'+i+'">Show Data</button></td><td>'+tracks[i].numPoints+'</td><td>'+tracks[i].len+'</td><td>'+tracks[i].loop+'</td></tr>');
                        }

                        $('#viewPanel').on('click', 'button[id^="tracks"]', function (e) {
                        
                            let id = this.id; 
                            let a = id.slice(6);
                            alert(a);
                            let y = tracks[a].otherData.length;
                            //alert(len);
                            //alert(x+len);
                            if (y === 0) {
                                alert("No other Data");
                            } else {
                                for (let i = 0; i < y; i++) {
                                    var str = tracks[a].otherData[i].name + ":" + tracks[a].otherData[i].value + " ";
                                }
        
                               alert(str);
                            }
                            
                        });
                    }

   

                    $('#clearData').click(function() {
                        $("#viewPanel tr").empty();
                    });
                    
                    // console.log(routes.length);
                }, 
                fail: function(error) {
                    console.log(error);
                }
            });
        }
        
    });


    $("#fsAdd").change(function() {
        var input = $(this).children("option:selected").val();
        var wptObjs = [];
        var counter = 0;
    
        if(input !== "Select File") {
            $('#reset').click(function() { 
                var lat = $("#lat").val();
                var lon = $("#lon").val();
    
                var emptyCheck = true;
                var nanCheck = true;
                var lonInRange = true;
                var latInRange = true;
    
                if (lat === "" || lon === "") {
                    alert("Can not leave feilds empty");
                    emptyCheck = false;
                }
    
                if(isNaN(lat) || isNaN(lon)) {
                    alert("All values must be a real number");
                    nanCheck = false;
                } 
    
    
                if (lat < -90 || lat > 90) {
                    alert("Value of Lat must be between -90 to 90");
                    latInRange = false;
                }
    
        
                if (lon < -180 || lon > 180) {
                    alert("Value of Lon must be between 180- to 180");
                    latInRange = false;
                }
    
                if (latInRange === true && lonInRange === true && nanCheck === true && emptyCheck === true) {
                    let obj = {"lat":lat, "lon":lon};
                    wptObjs.push( obj);
                    console.log(wptObjs[counter]);
                    alert("Added Wpt, delete values and enter new ones for Lat and Lon if you want to another wpt to this route. Or, click add to route to file");

                    counter++;
                
                }
    
            });

            $('#submitRte').click(function() {
                var name = $("#retName").val();
                if(name === "") {
                    alert("name can not be empty")
                } else {
                    jQuery.ajax({
                        type: 'get',            
                        dataType: 'json',      
                        url: '/addRoute',   
                        data: {
                            file:input,
                            name:name,
                            wpts:wptObjs,
                            length:wptObjs.length
                        },
        
                        success: function (data) {
                            if(data.rte === 0 || data.wpt === 0) {
                                alert("Route Failed to add");
                            } else {
                                //alert("Route Added");
                                location.reload(true);
                            }
                        
                        },
                        fail: function(error) {
                            console.log(error); 
                        }
                    });
                }
            });
        }
    });
           


        $('#getLen').click(function() { 
            var sourceLat = $("#sourceLat").val();
            var sourceLon = $("#sourceLon").val();
            var destLon = $("#destLon").val();
            var destLat = $("#destLat").val();
            var delta = $("#delta").val();

            var emptyCheck = true;
            var nanCheck = true;
            var lonInRange = true;
            var latInRange = true;

            if (sourceLat === "" || sourceLon === "" || destLat === "" || destLon === "" || delta === "") {
                alert("Can not leave feilds empty");
                emptyCheck = false;
            }

            if(isNaN(sourceLat) || isNaN(sourceLon) || isNaN(destLon) || isNaN(destLat) || isNaN(delta)) {
                alert("All values must be a real number");
                nanCheck = false;
            } 

            if (sourceLat < -90 || sourceLat > 90 || delta < 0) {
                alert("Value of Lat must be between -90 to 90");
                latInRange = false;
            }

            if (destLat < -90 || destLat > 90) {
                alert("Value of Lat must be between -90 to 90");
                latInRange = false;
            }

            if (destLon < -180 || destLon > 180) {
                alert("Value of Lon must be between 180- to 180");
                lonInRange = false;
            }

            if (sourceLon < -180 || sourceLon > 180) {
                alert("Value of Lon must be between 180- to 180");
                latInRange = false;
            }

            if (latInRange === true && lonInRange === true && nanCheck === true && emptyCheck === true) {
                //alert(sourceLat + " " + sourceLon + " " + destLat + " " + destLon + " " + delta);
                jQuery.ajax({
                    type: 'get',            //Request type
                    dataType: 'json',       //Data type - we will use JSON for almost everything 
                    url: '/getPathsBetween',   //The server endpoint we are connecting to
                    data: {
                        sLat:sourceLat,
                        sLon:sourceLon,
                        dLat:destLat,
                        dLon:destLon,
                        delta:delta
                    },

                    success: function (data) {
                        $('#paths').append('<tr><th>Component</th><th>name</th><th>numPoints</th><th>Length</th><th>Loop</th></tr>');
                        console.log(data.routesIn);
                        for (let i = 0; i < data.routesIn.length; i++){
                            let arr = JSON.parse(data.routesIn[i]);
                            if (arr.length !== 0) {
                                console.log(arr.length);
                                for (let j = 0; j < arr.length; j++) {
                                    $('#paths').append('<tr><td>Route</td><td>'+arr[j].name+'</td><td>'+arr[j].numPoints+'</td><td>'+arr[j].len+'</td><td>'+arr[j].loop+'</td></tr>');         
                                }
                            }           
                         }
                         
        
                         for (let i = 0; i < data.tracksIn.length; i++){
                            let arr = JSON.parse(data.tracksIn[i]);
                            if (arr.length !== 0) {
                                for (let j = 0; j < arr.length; j++) {
                                    $('#paths').append('<tr><td>Track</td><td>'+arr[j].name+'</td><td>'+arr[j].numPoints+'</td><td>'+arr[j].len+'</td><td>'+arr[j].loop+'</td></tr>');         
                                }
                            }           
                         }
                    
                    },
                    fail: function(error) {
                        console.log(error); 
                    }
                });
            
            } 

        });
        

    $('#browse').change(function(e) { 
        let input = document.getElementById('browse');
        let file = input.files[0].name;

        jQuery.ajax({
            type: 'get',            //Request type
            dataType: 'json',       //Data type - we will use JSON for almost everything 
            url: '/validateFile',   //The server endpoint we are connecting to
            data: {
                fileName:file
            },
            success: function (data) {
                 console.log("In index:" + data.state);

                if (data.state === true) {
                    alert("Invalid file, this file either exists on the server");
                    input.value = '';
                }
                
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });
    });



    $('#submitGPX').click(function() { 
        var fileState = false;
        var creatState = false;
        var versionState = false;
        
        var fileName = $("#fName").val();
        if (fileName !== "") {
            var extension = fileName.split(".");
            var len = extension.length-1;
            if (len >= 0) {
                if(extension[len] === "gpx") {
                    fileState = true;
                } else {
                    alert("Invalid file format");
                }
            }
        }

        let version = $("#version").val();
        if(isNaN(version)) {
            alert("version must be a number");
        } else {
            versionState = true;
        }

        let creator = $("#creator").val();
        if (creator === "") {
            alert("Creator feild it empty");
        } else {
            creatState = true;
        }


        if (creatState === true && fileState === true && versionState === true) {
           //alert(fileState+versionState+creatState);
            // // alert("Form validated");
             jQuery.ajax({
                type: 'get',            //Request type
                dataType: 'json',       //Data type - we will use JSON for almost everything 
                url: '/validateFile',   //The server endpoint we are connecting to
                data: {
                    fileName:fileName
                },
                success: function (data) {
                     console.log("In index:" + data.state);
    
                    if (data.state === true) {
                        alert("Invalid file, this file either exists on the server");
                    } else {
                        jQuery.ajax({
                            type: 'get',            //Request type
                            dataType: 'json',       //Data type - we will use JSON for almost everything 
                            url: '/writeFile',   //The server endpoint we are connecting to
                            data: {
                                fileName:fileName,
                                creator:creator,
                                version:version
                            },
                            success: function (data) {
                                if (data === true) {
                                    location.reload();
                                }
                                
                            },
                            fail: function(error) {
                                console.log(error); 
                            }
                        });
                    }     
                },
                fail: function(error) {
                    console.log(error); 
                }
            });
        } 

        //alert(fileName+version+creator);
        
        
    });



    //event listner for downloading the file
    $('#simpleDownload').click(function(e){
        console.log("Downloaded simple.gpx");
        //alert("Chnaged Route: "+$('#routeNameEB').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });


    //event listner for change route name feild
    $('#routeName').submit(function(e){
        console.log("Changed Route Name");
        alert("Chnaged Route: "+$('#routeNameEB').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });

    //event listner for the track name change feild 

    $('#trackName').submit(function(e) {
        console.log("Changed Track Name");
        alert("Changed Track name to: "+$('#trackNameEB').val());
        e.preventDefault();
        //Pass data to the Ajax call, so it gets passed to the server
        $.ajax({
            //Create an object for connecting to another waypoint
        });
    });



/*** Assignment 4 Functionality */

$('#login').click(function() { 
    var user = false;
    var pass = false;
    var name = false;

    let username = $("#username").val();
    if (username !== "") {
        user = true;
    }

    let databaseName = $("#databaseName").val();
    if (databaseName !== "") {
        name = true;
    }

    let password = $("#password").val();
    if (password !== "") {
        pass = true;
    }

    if (pass === true && user === true && name === true ) {
        
         jQuery.ajax({
            type: 'get',           //Request type
            dataType: 'json',   //Data type - we will use JSON for almost everything 
            url: '/login',   //The server endpoint we are connecting to
            data: {
                username:username,
                password:password,
                databasename: databaseName
            },
            success: function (data) {
                if(data.state === 1) {
                    alert("Login successful");
                    $("#showDBCmds").show();
                    populateRouteLog();
                    populateQFLogs();
                    $('#storeFiles').click(function() {
                      
                        $.ajax({
                            type: 'get',
                            dataType: 'json',
                            url: '/fileLogPanel',
                            data : {},
                            success: function (data) {
                                if (data.fileNum > 0) {   
                                    jQuery.ajax({
                                        type: 'get',
                                        dataType: 'json',
                                        url: '/storeFiles',
                                        data : {
                                            fileData:data.gpxdocs,
                                            numFiles:data.fileNum
                                        },
                                        success: function (data) {
                                            if (data.state === 1) {
                                                alert("Files loaded onto the server");
                                                populateRouteLog();
                                                populateQFLogs();
                                            }   
                                        },
                                        fail: function (error) {
                                            console.log(error);
                                        }
                                    });
                                } else {
                                    alert("There are no files no server, can not store in Data Base");
                                }
                            }, 
                            fail: function(error) {
                                console.log(error); 
                            }
                
                        }); 
                    });

                    //display database status
                    $('#dbStatus').click(function() {
                        jQuery.ajax({
                            type: 'get',
                            dataType: 'json',
                            url: '/databaseStatus',
                            data : {},
                            success: function (data) {
                                alert("NumFiles: " + data.ret[0] + " " + "NumRoutes: " + data.ret[1] + " " + "NumPoints: " + data.ret[2]);
                            },
                            fail: function (error) {
                                console.log(error);
                            }
                        });
                    });

                    //clear database status
                    $('#clearFiles').click(function() {
                        jQuery.ajax({
                            type: 'get',
                            dataType: 'json',
                            url: '/clearDatabase',
                            data : {},
                            success: function (data) {
                                if (data.state === 1) {
                                    alert("Cleared data base");
                                    location.reload();
                                }
                            },
                            fail: function (error) {
                                console.log(error);
                            }
                        });
                    });


                } else {
                    alert("login Failed, please enter the correct credentials");
                }
            },
            fail: function(error) {
                console.log(error); 
            }
        });
    } 
});

    function populateRouteLog() {   
        jQuery.ajax({
            type: 'get',            
            dataType: 'json',       
            url: '/routePanel',  
            data: {},
            success: function (data) {
                //console.log(data);
                $('#queryRs').empty();
                for (let row of data.records) {
                    $('#queryRs').append('<option id =' + row.route_id+'>' +row.route_name+' </option>');
                }
                
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });

    };

    function populateQFLogs() {   
        jQuery.ajax({
            type: 'get',            
            dataType: 'json',       
            url: '/qFilePanel',  
            data: {},
            success: function (data) {
                //console.log(data);
                $('#queryFs').empty();
                $('#query4Fs').empty();
                $('#query5Fs').empty();
                for (let row of data.records) {
                    $('#queryFs').append('<option id =' + row.file_name+'>' + row.file_name+' </option>');
                    $('#query4Fs').append('<option id =' + row.file_name+'>' + row.file_name+' </option>');
                    $('#query5Fs').append('<option id =' + row.file_name+'>' + row.file_name+' </option>');
                }
                
            },
            fail: function(error) {
                // Non-200 return, do something with error
                console.log(error); 
            }
        });

    };


                                
    $('#q1N').click(function() {
        jQuery.ajax({
            type: 'get',
            dataType: 'json',
            url: '/q1',
            data : {
                qType: "name"
            },
            success: function (data) {
                $('#qData').append('<tr><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');

                for (let row of data.records) {
                    $('#qData').append('<tr><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');                   
                }
            },
            fail: function (error) {
                console.log(error);
            }
        });

    });

    $('#q1L').click(function() {
        jQuery.ajax({
            type: 'get',
            dataType: 'json',
            url: '/q1',
            data : {
                qType: "length"
            },
            success: function (data) {
                $('#qData').append('<tr><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');

                for (let row of data.records) {
                    $('#qData').append('<tr><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');                   
                }
            },
            fail: function (error) {
                console.log(error);
            }
        });

    });

    $('#q2L').click(function() {
        let input = $("#queryFs").children("option:selected").val();
        if (input === "Select File") {
            alert ("Please Chose A File to execute this query");
        } else {
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q2',
                data : {
                    qType: "length",
                    fileName: input
                },
                success: function (data) {
                    $('#qData').append('<tr><th>file name</th><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
        
                for (let row of data.records) {
                    $('#qData').append('<tr><td>'+input+'</td><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');                   }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });

    $('#q2N').click(function() {
        let input = $("#queryFs").children("option:selected").val();
        if (input === "Select File") {
            alert ("Please Chose A File to execute this query");
        } else {
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q2',
                data : {
                    qType: "name",
                    fileName: input
                },
                success: function (data) {
                    $('#qData').append('<tr><th>file Name</th><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
        
                for (let row of data.records) {
                    $('#qData').append('<tr><td>'+input+'</td><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');                   
                }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });

    $('#q3').click(function() {
        let input = $("#queryRs").children("option:selected").val();
        let id = $("#queryRs").children("option:selected").attr('id');
        if (input === "Select Route") {
            alert("Please Select a Route");
        } else {
           // alert(input+id);
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q3',
                data : {
                    rte_id: id
                },
                success: function (data) {
                $('#qData').append('<tr><th>point_id</th><th>point_index</th><th>latitude</th><th>longitude</th><th>point_name</th><th>route_id</th></tr>');
                alert(data.records.length);
                for (let row of data.records) {
                    //console.log(row.latitude + " " + row.longitude);
                    $('#qData').append('<tr><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td>'+row.point_name+'</td><td>'+row.route_id+'</td></tr>');  
                }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });

    $('#q4L').click(function() {
        let input = $("#query4Fs").children("option:selected").val();
        if (input === "Select File") {
            alert ("Please Chose A File to execute this query");
        } else {
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q4',
                data : {
                    qType: "length",
                    fileName: input
                },
                success: function (data) {
                    $('#qData').append('<tr><th>point_id</th><th>point_index</th><th>latitude</th><th>longitude</th><th>point_name</th><th>route_id</th></tr>');
                
                let counter = 0;
                for (let row of data.records) {
                    if(row.point_name === null) {
                        $('#qData').append('<tr><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td> No Name Route '+counter+'</td><td>'+row.route_id+'</td></tr>'); 
                        counter++; 
                    } else {
                        $('#qData').append('<tr><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td>'+row.point_name+'</td><td>'+row.route_id+'</td></tr>');  

                    }
                }                  
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });

    $('#q4N').click(function() {
        let input = $("#query4Fs").children("option:selected").val();
        if (input === "Select File") {
            alert ("Please Chose A File to execute this query");
        } else {
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q4',
                data : {
                    qType: "name",
                    fileName: input
                },
                success: function (data) {
                    $('#qData').append('<tr><th>point_id</th><th>point_index</th><th>latitude</th><th>longitude</th><th>point_name</th><th>route_id</th></tr>');
                
                let counter = 0;
                for (let row of data.records) {
                    if(row.point_name === null) {
                        $('#qData').append('<tr><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td> No Name Route '+counter+'</td><td>'+row.route_id+'</td></tr>'); 
                        counter++; 
                    } else {
                        $('#qData').append('<tr><td>'+row.point_id+'</td><td>'+row.point_index+'</td><td>'+row.latitude+'</td><td>'+row.longitude+'</td><td>'+row.point_name+'</td><td>'+row.route_id+'</td></tr>');  

                    }                }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });

    $('#q5L').click(function() {
        let input = $("#query5Fs").children("option:selected").val();
        let numRts = $("#numRts").val();
        let nameSort = document.getElementById("sortName").checked;

        if (input === "Select File" || numRts === "" ) {
            alert ("Please Chose A File to execute this query and make sure you provide the number of routes");
        } else {
           // alert("success");
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q5',
                data : {
                    qType: "longest",
                    fileName: input,
                    limit:numRts,
                    nameSort:nameSort
                },
                success: function (data) {
                    $('#qData').append('<tr><th>file name</th><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
        
                    for (let row of data.records) {
                        $('#qData').append('<tr><td>'+input+'</td><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');                   
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });

    $('#q5S').click(function() {
        let input = $("#query5Fs").children("option:selected").val();
        let numRts = $("#numRts").val();
        let nameSort = document.getElementById("sortName").checked;

        if (input === "Select File" || numRts === "") {
            alert ("Please Chose A File to execute this query and make sure you provide the number of routes");
        } else {
            //alert("success");
            jQuery.ajax({
                type: 'get',
                dataType: 'json',
                url: '/q5',
                data : {
                    qType: "shortest",
                    fileName: input,
                    limit:numRts,
                    nameSort:nameSort
                },
                success: function (data) {
                    $('#qData').append('<tr><th>file name</th><th>route_id</th><th>route_name</th><th>route_len</th><th>gpx_id</th></tr>');
        
                    for (let row of data.records) {
                        $('#qData').append('<tr><td>'+input+'</td><td>'+row.route_id+'</td><td>'+row.route_name+'</td><td>'+row.route_len+'</td><td>'+row.gpx_id+'</td></tr>');                   
                    }
                },
                fail: function (error) {
                    console.log(error);
                }
            });
        }
        
    });



    $('#clearDataQ').click(function() {
        $("#qData tr").remove();
    });

});

