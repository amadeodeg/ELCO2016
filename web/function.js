var tempRows;
var presRows;
var humiRows;
var UVRows;
var rainRows;
var wispRows;
var widiRows;

function plot24h(){
	plotntime(86400);
}

function plot7d(){
	plotntime(604800);
}

function plotntime(ntime){
	$("#divTemperature").html("");
	$("#divPresure").html("");
	$("#divHumidity").html("");
	$("#divUV").html("");
	$("#divRain").html("");
	$("#divWindSpeed").html("");
	$("#divWindDirection").html("");
	if(document.getElementById("chkTemp").checked){
		plotPropertie("Temperatura ºC", tempRows, "divTemperature", ntime, "#0000ff");
	} 
	if(document.getElementById("chkPres").checked){
		plotPropertie("Presión mbar", presRows, "divPresure", ntime, "#00ff00");
	}
	if(document.getElementById("chkHumi").checked){
		plotPropertie("Humedad %", humiRows,"divHumidity", ntime, "#ff0000");
	}
	if(document.getElementById("chkUV").checked){
		plotPropertie("UV mW/m2", UVRows, "divUV", ntime, "#00ffff");
	} 
	if(document.getElementById("chkRain").checked){
		plotPropertie("Lluvia l/m2", rainRows, "divRain", ntime, "#ffff00");
	}
	if(document.getElementById("chkWiSp").checked){
		plotPropertie("Velocidad del viento", wispRows,"divWindSpeed", ntime, "#ff00ff");
	}
	if(document.getElementById("chkWiDi").checked){
		plotPropertie("Dirección del viento", widiRows,"divWindDirection", ntime, "#545454");
	}
}

function plotPropertie(title, dataFile, divId, timeBefore, colorLine){
	//d3.csv(dataFile, function(error, rows){
		var rows = dataFile;

		var maxDateAxis = rows[rows.length-1].x;
		var minDateAxis = maxDateAxis - timeBefore;

		var jump = Math.ceil((maxDateAxis - minDateAxis)/86400);

		rows = rows.filter(function(d){
    		return (d.x>=minDateAxis && d.x<=maxDateAxis);
    	});

    	rows = rows.filter(function(d){
    		return (d.x%(jump*60*5)==0)
    	});

    	rows = factorData(rows);

		nv.addGraph(function() {
			var chart = nv.models.lineChart();
    		chart.xAxis.rotateLabels(-75);	 
    		chart.margin().bottom = 150;
    		var data = [{
				key: "",
				values: rows,
				color: colorLine
			}];


    		chart.xAxis
				.axisLabel("Hora-Fecha");

			chart.yAxis
				.axisLabel(title);

			chart.xAxis.tickFormat(function(d) {
				return d3.time.format('%H:%M - %d/%m/%y')(new Date(d*1000));
			});

			// chart.forceY([
			// 	0.5*_.min(rows,function(d){return +d.y}).y,1.2*_.max(rows,function(d){return +d.y}).y
			// ]);

			d3.select("#"+divId).append("svg")
			.attr("height",400)
				.datum(data)
				.transition()
				.duration(500)
				.call(chart);

			nv.utils.windowResize(
				function(){
					chart.update();
				});

			chart.yAxis.scale().nice();

			return chart;
		});
	//});
}

function factorData(rows){
    		var result= [];
    		for (var i = 0; i <rows.length; i++) {
    			result[i]={x: rows[i].x,  y: rows[i].y/100};
    		}
    		return result;
}

function plotuserconf(){
	$("#divTemperature").html("");
	$("#divPresure").html("");
	$("#divHumidity").html("");
	$("#divUV").html("");
	$("#divRain").html("");
	$("#divWindSpeed").html("");
	$("#divWindDirection").html("");
	var minDateAxis=getMinDateAxis();
	var maxDateAxis=getMaxDateAxis(); 
	if(document.getElementById("chkTemp").checked){
		plotPropertie2("Temperatura ºC", tempRows, "divTemperature", minDateAxis, maxDateAxis, "#0000ff");
	} 
	if(document.getElementById("chkPres").checked){
		plotPropertie2("Presión mbar", presRows, "divPresure", minDateAxis, maxDateAxis, "#00ff00");
	}
	if(document.getElementById("chkHumi").checked){
		plotPropertie2("Humedad %", humiRows,"divHumidity", minDateAxis, maxDateAxis, "#ff0000");
	}
	if(document.getElementById("chkUV").checked){
		plotPropertie("UV mW/m2", UVRows, "divUV", minDateAxis, maxDateAxis, "#00ffff");
	} 
	if(document.getElementById("chkRain").checked){
		plotPropertie("Lluvia l/m2", rainRows, "divRain", minDateAxis, maxDateAxis, "#ffff00");
	}
	if(document.getElementById("chkWiSp").checked){
		plotPropertie("Velocidad del viento", wispRows,"divWindSpeed", minDateAxis, maxDateAxis, "#ff00ff");
	}
	if(document.getElementById("chkWiDi").checked){
		plotPropertie("Dirección del viento", widiRows,"divWindDirection", minDateAxis, maxDateAxis, "#545454");
	}
}


function plotPropertie2(title, dataFile, divId, minDateAxis, maxDateAxis, colorLine){
	//d3.csv(dataFile, function(error, rows){
		var rows = dataFile;

		var jump = Math.ceil((maxDateAxis - minDateAxis)/86400);

		rows = rows.filter(function(d){
    		return (d.x>=minDateAxis && d.x<=maxDateAxis);
    	});

    	rows = rows.filter(function(d){
    		return (d.x%(jump*60*5)==0)
    	});

    	rows = factorData(rows);

		nv.addGraph(function() {
			var chart = nv.models.lineChart();
    		chart.xAxis.rotateLabels(-75);	 
    		chart.margin().bottom = 150;
    		var data = [{
				key: "",
				values: rows,
				color: colorLine
			}];

    		chart.xAxis
				.axisLabel("Hora-Fecha");

			chart.yAxis
				.axisLabel(title);

			chart.xAxis.tickFormat(function(d) {
				return d3.time.format('%H:%M - %d/%m/%y')(new Date(d*1000));
			});

			// chart.forceY([
			// 	0.5*_.min(rows,function(d){return +d.y}).y,1.2*_.max(rows,function(d){return +d.y}).y
			// ]);

			d3.select("#"+divId).append("svg")
			.attr("height",400)
				.datum(data)
				.transition()
				.duration(500)
				.call(chart);

			nv.utils.windowResize(
				function(){
					chart.update();
				});

			chart.yAxis.scale().nice();

			return chart;
		});
	//});
}

function getMinDateAxis(){
	var minDate = document.getElementById("minDate").value?document.getElementById("minDate").value:"1970-01-01";
	var minTime = document.getElementById("minTime").value?document.getElementById("minTime").value:"00:00";
	return (new Date(minDate+" "+minTime))/1000;
}

function getMaxDateAxis(){
	var maxDate = document.getElementById("maxDate").value?document.getElementById("maxDate").value:"9999-12-31";
	var maxTime = document.getElementById("maxTime").value?document.getElementById("maxTime").value:"23:59";
	return (new Date(maxDate+" "+maxTime))/1000;
}

function loadFiles(){
	d3.csv("./data/Temperature.csv", function(error, rows){
		tempRows = rows;});

	d3.csv("./data/Humidity.csv", function(error, rows){
		humiRows = rows;});

	d3.csv("./data/Presure.csv", function(error, rows){
		presRows = rows;});

	d3.csv("./data/UV.csv", function(error, rows){
		UVRows = rows;});

	d3.csv("./data/Rain.csv", function(error, rows){
		rainRows = rows;});

	d3.csv("./data/WindSpeed.csv", function(error, rows){
		wispRows = rows;});

	d3.csv("./data/WindDirection.csv", function(error, rows){
		widiRows = rows;});

}

loadFiles();


