import React, { Component } from 'react';
import './App.css';
import '../node_modules/react-vis/dist/style.css';

import {XYPlot, LineSeries, MarkSeries, VerticalGridLines, HorizontalGridLines, XAxis, YAxis} from 'react-vis';

import * as axios from 'axios';

import 'bootstrap/dist/css/bootstrap.min.css';

const classPalate = [
  "#e41a1c",
  "#377eb8",
  "#4daf4a",
  "#984ea3",
  "#ff7f00",
  "#ffff33",
  "#a65628",
  "#f781bf",
  "#999999",
];

class CheckBox extends Component {
  constructor(props) {
    super(props);
    this.handleChange = this.handleChange.bind(this);
  }

  handleChange(e) {
    this.props.onCheckChange(e);
  }

  render() {
    return (
      <div>
        <input id={this.props.id} type="checkbox" checked={this.props.checked} onChange={this.handleChange}/>
        <label htmlFor={this.props.id}>{this.props.label}</label>
      </div>
    );
  }
}

class ChoiceList extends Component {
  constructor(props) {
    super(props);
    this.handleSelectionChange = this.handleSelectionChange.bind(this);
  }

  handleSelectionChange(e) {
    this.props.onQueryChange(e);
  }

  render() {
    return (
      <div className="ChoiceList">
        {Object.keys(this.props.data).map( (d) =>
                                           <CheckBox
                                               id={d}
                                               label={'Category '+d}
                                               key={d}
                                               checked={this.props.data[d]}
                                               onCheckChange={this.handleSelectionChange} />
                                         )}
      </div>
    );
  }
}

class MeshPlot extends Component {

  render() {
    const defaultStyle = {
      display: 'inline-block'
    };

    return (
      <div className="MeshPlot" style={defaultStyle}>
        <XYPlot height={this.props.height} width={this.props.width} xDomain={this.props.xDomain} yDomain={this.props.yDomain}>
          {Object.keys(this.props.data).map(
            (k) => <MarkSeries key={k} data={this.props.data[k]} color={classPalate[k]} size={this.props.size}/>
          )}
      </XYPlot>
        </div>
    );
  }
}

class PersistanceDiagram extends Component {
  render() {
    // find the largest value so we can draw a proper diagonal line
    let maxValue = -1; // the value of a node in PD will never be negative
    for(let k of Object.keys(this.props.data)) {
      for(let d of this.props.data[k]) {
        if(d.x > maxValue) maxValue = d.x;
        if(d.y > maxValue) maxValue = d.y;
      }
    }
    if(maxValue === -1) maxValue = 1;
    maxValue = 1.2*maxValue; // add some padding

    const defaultStyle = {
      display: 'inline-block'
    };

    return (
      <div className="PersistanceDiagram" style={defaultStyle}>
        <XYPlot height={this.props.height} width={this.props.width}>
          <VerticalGridLines />
          <HorizontalGridLines />
          <XAxis />
          <YAxis />
          <LineSeries data={[{x:0, y:0}, {x:maxValue, y:maxValue}]} color='#636363' />
          {Object.keys(this.props.data).map( (k) =>
                                             <MarkSeries data={this.props.data[k]} key={k} />
                                           )}
      </XYPlot>
        </div>
    );
  }
}

class App extends Component {
  constructor(props) {
    super(props);
    this.handleQueryChange = this.handleQueryChange.bind(this);
    this.queryPersistenceDiagram = this.queryPersistenceDiagram.bind(this);

    this.state = {
      mesh: [],
      mesh_domain: {},
      pd: [],
      query: {}
    };
  }

  handleQueryChange(e) {
    let q = this.state.query;
    q[e.target.id] = e.target.checked;
    this.setState({pd:[], query: q}); // reset pd and update query
    this.queryPersistenceDiagram();
  }

  queryPersistenceDiagram() {
    let func1 = function() {
      let sendToCubes = {points: []};
      let queriedMesh = this.getQueriedMesh(this.state.mesh, this.state.query);
      for(let k of Object.keys(queriedMesh)) {
        for(let d of queriedMesh[k]) {
          sendToCubes.points.push({px: d.x, py: d.y, c: Number(k)});
        }
      }

      axios.post('http://localhost:8800/query', JSON.stringify(sendToCubes))
        .then(function (response) {
          let rawdata = response.data;
          let data = {};
          rawdata.split('\n').map( (row) => {
            let line = row.split(' ');
            if (line.length === 3) {
              data[line[0]] = data[line[0]] || [];
              data[line[0]].push({x: Number(line[1]), y: Number(line[2])});
            }
            return true;
          });
          this.setState({pd: data});
        }.bind(this))
        .catch(function (error) {
          console.log(error);
        });
    }.bind(this);

    let func2 = function() {
      let queriedCategories = [];
      for(let k of Object.keys(this.state.query)) {
        if(this.state.query[k] === true) {
          queriedCategories.push(Number(k));
        }
      }


      axios.post('http://localhost:8800/query2', JSON.stringify(queriedCategories))
        .then(function (response) {
          let rawdata = response.data;
          let data = {};
          rawdata.split('\n').map( (row) => {
            let line = row.split(' ');
            if (line.length === 3) {
              data[line[0]] = data[line[0]] || [];
              data[line[0]].push({x: Number(line[1]), y: Number(line[2])});
            }
            return true;
          });
          console.log(data);
          this.setState({pd: data});
        }.bind(this))
        .catch(function (error) {
          console.log(error);
        });

    }.bind(this);

    func2();
  }

  getQueriedMesh(original, query) {
    let queriedData = {};
    for(let k of Object.keys(original)) {
      if(query[k] === true) {
        queriedData[k] = original[k];
      }
    }
    return queriedData;
  }

  componentDidMount() {
    // TODO more efficient way to parse data
    let parseData = function(rawdata) {
      let data = [];
      let dim = rawdata.schema.length;
      for(let count = 0; count < rawdata.pointcloud.length; count ++) {
        let row = {};
        for(let i = 0; i < dim; i ++) {
          row[rawdata.schema[i]] = rawdata.pointcloud[count][i];
        }
        data.push(row);
      }

      let parsedData = {};
      data.map( (d) => {
        parsedData[d.c] = parsedData[d.c] || [];
        return parsedData[d.c].push({x: Number(d.px), y: Number(d.py)})
      });

      let xDomain = [], yDomain = []; // calculate the global domain of the mesh
      xDomain.push( 0.9*Math.min(...data.map( (d) => Number(d.px))) );
      xDomain.push( 1.05*Math.max(...data.map( (d) => Number(d.px))) );
      yDomain.push( 0.9*Math.min(...data.map( (d) => Number(d.py))) );
      yDomain.push( 1.05*Math.max(...data.map( (d) => Number(d.py))) );

      // by default, query all data
      let q = {};
      for(let k of Object.keys(parsedData)) {
        q[k] = true;
      }

      this.setState({mesh: parsedData, mesh_domain:{xDomain: xDomain, yDomain: yDomain}, query: q});
      this.queryPersistenceDiagram();
    }.bind(this);

    // query for original point cloud
    axios.get('http://localhost:8800/pointcloud')
      .then(function (response) {
        parseData(response.data);
      })
      .catch(function (error) {
        console.log(error);
      });
  }

  render() {
    const queriedMesh = this.getQueriedMesh(this.state.mesh, this.state.query);

    const meshPanel = (
      <div className="panel panel-default">
        <div className="panel-heading">Mesh</div>
        <div className="panel-body">
          <MeshPlot
            height={500} width={500}
            data={queriedMesh || []}
            xDomain={this.state.mesh_domain.xDomain}
            yDomain={this.state.mesh_domain.yDomain}
            size={10}
            />
        </div>
      </div>
    );

    const queryPanel = (
      <div className="panel panel-default">
        <div className="panel-heading">Query</div>
        <div className="panel-body">
          <ChoiceList data={this.state.query} onQueryChange={this.handleQueryChange}/>
        </div>
      </div>
    );

    const pdPanel = (
      <div className="panel panel-default">
        <div className="panel-heading">Persistance Diagram</div>
        <div className="panel-body">
          <PersistanceDiagram data={this.state.pd} height={300} width={300}/>
        </div>
      </div>
    );

    const mainContent = (
      <div className="container">
        <div className="row justify-content-center">

          <div className="col-md-7">{meshPanel}</div>

          <div className="col-md-5">
            <div className="row">
              <div className="col">{queryPanel}</div>
            </div>
            <div className="row">
              <div className="col">{pdPanel}</div>
            </div>
          </div>

        </div>
      </div>
    );

    return (
      <div className="App">
        <div className="page-header">
          <h1>TopoCubes Viewer</h1>
        </div>
        <div>{mainContent}</div>
      </div>
    );
  }
}

export default App;

/* Local Variables:  */
/* mode: RJSX        */
/* js-indent-level: 2    */
/* End:              */
