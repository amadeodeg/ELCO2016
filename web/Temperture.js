function plotTemperture(){
d3.csv("data.csv", function(error, rows) { 
    	console.log(rows);


		var minDateAxis = getMinDateAxis();
		var maxDateAxis = getMaxDateAxis();

		rows = rows.filter(function(d){
    		return (d.x>=minDateAxis && d.x<=maxDateAxis);
    	});


    	nv.addGraph(function() {
    		var chart = nv.models.lineChart();
    		chart.xAxis.rotateLabels(-75);
    		chart.margin().bottom = 150;
    		var data = [{
				key: "Cosas",
				values: rows,
				color: "#0000ff"
			}];

    		chart.xAxis
				.axisLabel("Hora-Fecha");

			chart.yAxis
				.axisLabel("Temperatura");

			chart.xAxis.tickFormat(function(d) {
				return d3.time.format('%H:%M - %d/%m/%y')(new Date(d*1000));
			});


			chart.forceY([
				0.5*_.min(rows,function(d){return +d.y}).y,1.2*_.max(rows,function(d){return +d.y}).y
				]);


			d3.select("#grafico")
				.datum(data)
				.transition()
				.duration(500)
				.call(chart);

			nv.utils.windowResize(
				function(){
					chart.update();
				})


			chart.yAxis.scale().nice();

			return chart;



    	});
    });


}


function getMinDateAxis(){
	var minDate = document.getElementById("minDate").value?document.getElementById("minDate").value:"1970-01-01";
	var minTime = document.getElementById("minTime").value?document.getElementById("minTime").value:"00:00";
	//var a = new Date(minDate+" "+minTime);
	return (new Date(minDate+" "+minTime))/1000;
}

function getMaxDateAxis(){
	var maxDate = document.getElementById("maxDate").value?document.getElementById("maxDate").value:"9999-12-31";
	var maxTime = document.getElementById("maxTime").value?document.getElementById("maxTime").value:"23:59";
	//var a = new Date(maxDate+" "+maxTime);
	return (new Date(maxDate+" "+maxTime))/1000;
}