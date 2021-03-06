var catSelection = [true, true, true, true];
var original_data;

$(document).ready(function(){

    // calculation button
    $('#btn_cal').on('click', function () {

        d3.select('#persistenceplot').select('*').remove();

        var selected = [];
        for (var i = 0; i < original_data.length; i ++) {
            if (catSelection[original_data[i].c-1] == true)
                selected.push(original_data[i]);
        }

        var selection_id = '';
        for (var i = 0; i < catSelection.length; i ++) {
            if (catSelection[i] == true) {
                selection_id += String(i+1);
            }
        }

        if (selected.length > 0) {

            var info = {'points': selected, 'sel_id': selection_id};

            $.ajax({
                url: 'http://localhost:8800/query',
                method: 'POST',
                dataType: 'json',
                data: JSON.stringify(info),
                success: function(json) {
                    plotPersistence(json);
                }
            });
        }

    });


    // load data and plot
    d3.csv('mesh.csv', function(data) {

        // load data
        for (var i = 0; i < data.length; i ++) {
            data[i].px = Number(data[i].px);
            data[i].py = Number(data[i].py);
            data[i].c = Number(data[i].c);
        }

        original_data = data;
        plotScatter(data);

    });

});


function plotScatter(data) {

    // plot
    var margin = {top: 20, right: 20, bottom: 20, left: 20}
        , width = 500 - margin.left - margin.right
        , height = 500 - margin.top - margin.bottom;

    var xMax = d3.max(data, function(d) { return d.px; });
    var yMax = d3.max(data, function(d) { return d.py; });
    var totalMax = d3.max([xMax, yMax])+1;

    var x = d3.scaleLinear()
        .domain([0, totalMax])
        .range([ 0, width ]);

    var y = d3.scaleLinear()
        .domain([0, totalMax])
        .range([ height, 0 ]);

    var colorscale = d3.scaleOrdinal()
        .domain([1,2,3,4])
        .range(['#377eb8','#e41a1c','#4daf4a','#984ea3']);

    var chart = d3.select('#scatterplot')
        .append('svg:svg')
        .attr('width', width + margin.right + margin.left)
        .attr('height', height + margin.top + margin.bottom)
        .attr('class', 'chart')

    var main = chart.append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')')
        .attr('width', width)
        .attr('height', height)
        .attr('class', 'main')

    // draw the x axis
    var xAxis = d3.axisBottom(x);

    main.append('g')
        .attr('transform', 'translate(0,' + height + ')')
        .attr('class', 'main axis date')
        .call(xAxis);

    // draw the y axis
    var yAxis = d3.axisLeft(y);

    main.append('g')
        .attr('transform', 'translate(0,0)')
        .attr('class', 'main axis date')
        .call(yAxis);

    var g = main.append("svg:g");


    var plotDots = function (sel) {
        sel.append("circle")
            .attr("cx", function (d) { return x(d.px); } )
            .attr("cy", function (d) { return y(d.py); } )
            .attr("fill", function (d) { return colorscale(d.c); })
            .attr("r", 4);
    };

    g.selectAll("circle")
        .data(data)
        .enter()
        .call(plotDots);

    // selection part
    d3.selectAll("input").on('change', function () {
        catSelection[this.value-1] = this.checked ? true:false;

        d3.select('#scatterplot').selectAll("circle").remove();
        d3.select('#scatterplot').select('svg').select('g').selectAll("circle")
            .data(data)
            .enter()
            .filter(function(d) { return catSelection[d.c-1]; })
            .call(plotDots);
    });
}

function plotPersistence(json) {
    // parse json string
    var data = [];
    var lines = json.split('\n');
    for (var i = 0; i < lines.length; i ++) {
        var line = lines[i].split(' ');
        var d = {};
        if (line.length == 3) {
            d['c'] = Number(line[0]); // dim
            d['px'] = Number(line[1]); // birth
            d['py'] = Number(line[2]); // death
            data.push(d);
        }
    }

    // plot as scatter plot
    if(data.length == 0) {
        return;
    }

    var margin = {top: 20, right: 20, bottom: 20, left: 30}
        , width = 240 - margin.left - margin.right
        , height = 240 - margin.top - margin.bottom;

    var xMax = d3.max(data, function(d) { return d.px; });
    var yMax = d3.max(data, function(d) { return d.py; });
    var totalMax = d3.max([xMax, yMax])+1;

    var x = d3.scaleLinear()
        .domain([0, totalMax])
        .range([ 0, width ]);

    var y = d3.scaleLinear()
        .domain([0, totalMax])
        .range([ height, 0 ]);

    var colorscale = d3.scaleOrdinal()
        .domain([0,1,2,3])
        .range(['#1b9e77', '#d95f02', '#7570b3', '#e7298a']);

    var chart = d3.select('#persistenceplot')
        .append('svg:svg')
        .attr('width', width + margin.right + margin.left)
        .attr('height', height + margin.top + margin.bottom)
        .attr('class', 'chart')

    var main = chart.append('g')
        .attr('transform', 'translate(' + margin.left + ',' + margin.top + ')')
        .attr('width', width)
        .attr('height', height)
        .attr('class', 'main')

    // draw the x axis
    var xAxis = d3.axisBottom(x);

    main.append('g')
        .attr('transform', 'translate(0,' + height + ')')
        .attr('class', 'main axis date')
        .call(xAxis);

    // draw the y axis
    var yAxis = d3.axisLeft(y);

    main.append('g')
        .attr('transform', 'translate(0,0)')
        .attr('class', 'main axis date')
        .call(yAxis);

    var g = main.append("svg:g");


    var plotDots = function (sel) {
        sel.append("circle")
            .attr("cx", function (d) { return x(d.px); } )
            .attr("cy", function (d) { return y(d.py); } )
            .attr("fill", function (d) { return colorscale(d.c); })
            .attr("r", 4);
    };

    g.selectAll("circle")
        .data(data)
        .enter()
        .call(plotDots);
}
