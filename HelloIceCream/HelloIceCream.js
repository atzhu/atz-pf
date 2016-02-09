var canvas;
var gl;
var program; 
var vPosition;

var pointsArray = [[],[],[]];
var normalsArray = [[],[],[]];
var indices = [0,0,0];
var nBufferArray = [];
var vBufferArray = [];

// Movement Variables 
var fwdback 	= -30;
var cameralr 	= 0;
var cameraud 	= 0;
var fovy 		= 45;
var far 		= 1000;
var near 		= 0.1;
var turn 		= 0;
var scal 		= 0;
var count 		= 0;
var aspect;

var x = [];//[0,-5,-1,6,10];
var y = [];//[-5,15,15,15,15];
var z = [];//[0,0,0,0,0];
var c = [];//[1,0,0,0,0];
var p = []; //[0,1,2,3]; position of array

var coneX = 0;
var coneY = -5;
var coneZ = 0;
var coneC = 1;

var numScoops = 25;

// Shading Variables
var ambientColor, diffuseColor, specularColor;

//Viewing Variables
var modelViewMatrix, projectionMatrix;
var modelViewMatrixLoc, projectionMatrixLoc;
var normalMatrix, normalMatrixLoc;

// Make Cube
var cupointsArray = [[],[]];
var colorsArray = [];
var numVertices  = 36;
var vBuffer1;
var vBuffer2;

var cvertices =
[
    vec4( 0,-2, 0, 1),
    vec4(-1, 1, 1, 1),
    vec4( 1, 1, 1, 1),
    vec4( 0,-2, 0, 1),
    vec4( 0,-2, 0, 1),
    vec4(-1, 1,-1, 1),
    vec4( 1, 1,-1, 1),
    vec4( 0,-2, 0, 1)
];

var vertices = [
    vec4(-1, -1,  1, 1),
    vec4(-1,  1,  1, 1),
    vec4( 1,  1,  1, 1),
    vec4( 1, -1,  1, 1),
    vec4(-1, -1, -1, 1),
    vec4(-1,  1, -1, 1),
    vec4( 1,  1, -1, 1),
    vec4( 1, -1, -1, 1)
];

function quad(a, b, c, d, texarray, texcoord, vertice, cupointsArrays) {
     cupointsArrays.push(vertice[a]); 
     texarray.push(texcoord[0]);

     cupointsArrays.push(vertice[b]); 
     texarray.push(texcoord[1]); 

     cupointsArrays.push(vertice[c]); 
     texarray.push(texcoord[2]); 
   
     cupointsArrays.push(vertice[a]); 
     texarray.push(texcoord[0]); 

     cupointsArrays.push(vertice[c]); 
     texarray.push(texcoord[2]); 

     cupointsArrays.push(vertice[d]); 
     texarray.push(texcoord[3]);   
}

function colorCube(texarray, texcoord, vertice, cupointsArrays)
{
    quad(1, 0, 3, 2, texarray, texcoord, vertice, cupointsArrays);
    quad(2, 3, 7, 6, texarray, texcoord, vertice, cupointsArrays);
    quad(3, 0, 4, 7, texarray, texcoord, vertice, cupointsArrays);
    quad(6, 5, 1, 2, texarray, texcoord, vertice, cupointsArrays);
    quad(4, 5, 6, 7, texarray, texcoord, vertice, cupointsArrays);
    quad(5, 4, 0, 1, texarray, texcoord, vertice, cupointsArrays);
}
function resize(gl)
{
	var canvas = gl.canvas;
	
	var displayWidth  = canvas.clientWidth;
	var displayHeight = canvas.clientHeight;
 
	if (canvas.width  != displayWidth ||
      canvas.height != displayHeight)
	{
		canvas.width  = displayWidth;
		canvas.height = displayHeight;
		gl.viewport(0, 0, canvas.width, canvas.height);
	}
}

// Texture Map
var tBuffer = [];
var texCoordsArray = [[],[]];
var vTexCoord;

var texture = [];
var image = [];
var texSize = 32;

var texCoord =
[
    vec2( 0, 0),
    vec2( 0, 1),
    vec2( 1, 1),
    vec2( 1, 0)
];

// Tri linear mapping
function configureTexturetri(n) {
    texture[n] = gl.createTexture();
    gl.bindTexture( gl.TEXTURE_2D, texture[n] );
    gl.texImage2D( gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, image[n] );
    gl.generateMipmap( gl.TEXTURE_2D );
    gl.texParameteri( gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_LINEAR );
    gl.texParameteri( gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR );
    gl.uniform1i(gl.getUniformLocation(program, "texture"), 0);
}

//Lighting 
var lightPosition	= vec4( 0.0,10.0,20.0, 1.0);
var lightAmbient 	= vec4( 0.1, 0.1, 0.1, 1.0);
var lightDiffuse 	= vec4( 1.0, 0.9, 0.9, 1.0);
var lightSpecular	= vec4( 1.0, 1.0, 1.0, 1.0);

var colors =
[
    vec4( 1.0, 0.98, 0.7, 1.0 ),
    vec4( 0.7, 0.92, 1.0, 1.0 ),
	vec4( 0.73, 1.0, 0.7, 1.0 ),
	vec4( 0.45, 0.31, 0.094, 1.0 ),
    vec4( 1.0, 0.8, 1.0, 1.0 ),
	vec4( 1.0, 1.0, 1.0, 1.0 ),
	//vec4( 0.0, 0.0, 0.0, 1.0 )
];

var materialamb = [
	vec4( 0.3, 0.3, 0.3, 1.0 ),
	vec4( 0.0, 0.0, 1.0, 1.0 )
];

// color of light reflection
var materialdif = [];

var materialspec =
[
    vec4( 0, 0, 0, 1.0 ),
    vec4( 1.0, 1.0, 1.0, 1.0 )
];

var materialshin =
[
    50,
    50
];  

//Sphere Generation
var va = vec4( 0.0, 	 0.0,		-1.0,		1);
var vb = vec4( 0.0, 	 0.942809, 	 0.333333, 	1);
var vc = vec4(-0.816497,-0.471405, 	 0.333333, 	1);
var vd = vec4( 0.816497,-0.471405, 	 0.333333,	1);

// For flat shaded sphere - has one normal per triangle
function triangleflat(a, b, c, m)
{
    var t1 = subtract(b, a);
    var t2 = subtract(c, a);
    var normal = normalize(cross(t2, t1));
    normal = vec4(normal);
    normal[3]  = 0.0;

    normalsArray[m].push(normal);
    normalsArray[m].push(normal);
    normalsArray[m].push(normal);

    pointsArray[m].push(a);
    pointsArray[m].push(b);      
    pointsArray[m].push(c);

    indices[m] += 3;
}

// For smooth shaded sphere - has different normals per vertex
function trianglesmooth(a, b, c, m)
{
    pointsArray[m].push(a);
    pointsArray[m].push(b);      
    pointsArray[m].push(c);

    normalsArray[m].push(a[0],a[1], a[2], 0.0);
    normalsArray[m].push(b[0],b[1], b[2], 0.0);
    normalsArray[m].push(c[0],c[1], c[2], 0.0);

    indices[m] += 3;
}

function divideTriangle(a, b, c, count, flat, m)
{
    if (count > 0)
	{
        var ab = mix(a, b, 0.5);
        var ac = mix(a, c, 0.5);
        var bc = mix(b, c, 0.5);

        ab = normalize(ab, true);
        ac = normalize(ac, true);
        bc = normalize(bc, true);

        divideTriangle( a,ab,ac, count-1, flat, m);
        divideTriangle(ab, b,bc, count-1, flat, m);
        divideTriangle(bc, c,ac, count-1, flat, m);
        divideTriangle(ab,bc,ac, count-1, flat, m);
    }
    else
	{ 
        if (flat == 1) triangleflat(a, b, c, m);
        else trianglesmooth(a, b, c, m);
    }
}

function tetrahedron(a, b, c, d, n, flat, m)
{
    divideTriangle(a, b, c, n, flat, m);
    divideTriangle(d, c, b, n, flat, m);
    divideTriangle(a, d, b, n, flat, m);
    divideTriangle(a, c, d, n, flat, m);
}

    tetrahedron(va, vb, vc, vd, 2, 0, 0);	//0   smooth sphere
    tetrahedron(va, vb, vc, vd, 2, 1, 1);	//1   flat sphere
	// Bounding volumes
    tetrahedron(va, vb, vc, vd, 1, 0, 2);	//2 collision detection sphere // makes around 0-47 indices[2]

//FOR CUBES
function enableCube (t_Buffer, v_Buffer, n)
{
    // tell shaders it is a cube
    gl.uniform1i( gl.getUniformLocation(program, "firstv"), 1 );
    gl.uniform1i( gl.getUniformLocation(program, "firstf"), 1 );
	
    // bind the texture coordinates
    gl.bindBuffer( gl.ARRAY_BUFFER, t_Buffer);
    vTexCoord = gl.getAttribLocation( program, "vTexCoord" );
    gl.vertexAttribPointer( vTexCoord, 2, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vTexCoord );
	
    // bind the vertex coordinates
    gl.bindBuffer( gl.ARRAY_BUFFER, v_Buffer );
    //gl.bufferData( gl.ARRAY_BUFFER, flatten(cupointsArray), gl.STATIC_DRAW );
    vPosition = gl.getAttribLocation( program, "vPosition" );
    gl.vertexAttribPointer( vPosition, 4, gl.FLOAT, false, 0, 0 );
    gl.enableVertexAttribArray( vPosition ); 
	
    // bind texture image
    gl.bindTexture( gl.TEXTURE_2D, texture[n]);
    gl.texImage2D( gl.TEXTURE_2D, 0, gl.RGB, gl.RGB, gl.UNSIGNED_BYTE, image[n] );
}

// FOR SPHERES
function bindandlighting(j, color) {
	 //tell shader it is a sphere
    gl.uniform1i(gl.getUniformLocation(program, "firstv"), 0);
    gl.uniform1i(gl.getUniformLocation(program, "firstf"), 0);
	
    // tell shader to not pass texture coordinates
    gl.disableVertexAttribArray(vTexCoord);

    // bind the normal coordinates
    gl.bindBuffer(gl.ARRAY_BUFFER, nBufferArray[j]);
    vNormal = gl.getAttribLocation(program, "vNormal");
    gl.vertexAttribPointer(vNormal, 4, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vNormal);
	
    // bind the vertex coordinates
    gl.bindBuffer(gl.ARRAY_BUFFER, vBufferArray[j]);
    vPosition = gl.getAttribLocation(program, "vPosition");
    gl.vertexAttribPointer(vPosition, 4, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vPosition);
	
	// math for lighting 
    ambientProduct = mult(lightAmbient, materialamb[j]);
    diffuseProduct = mult(lightDiffuse, materialdif[color]);
    specularProduct = mult(lightSpecular, materialspec[j]);
	
    // pass lighting params to shader
    gl.uniform4fv(gl.getUniformLocation(program,"ambientProduct"),	flatten(ambientProduct));
    gl.uniform4fv(gl.getUniformLocation(program,"diffuseProduct"),	flatten(diffuseProduct));
    gl.uniform4fv(gl.getUniformLocation(program,"specularProduct"),	flatten(specularProduct));   
    gl.uniform4fv(gl.getUniformLocation(program,"lightPosition"),	flatten(lightPosition));
    gl.uniform1f (gl.getUniformLocation(program,"shininess"),		materialshin[j]);
}

function getRandomInt(min, max)
{
  return Math.floor(Math.random() * (max - min)) + min;
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////              init               ////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

window.onload = function init()
{
	initObjs();
    canvas = document.getElementById("gl-canvas");
    
    gl = WebGLUtils.setupWebGL(canvas);
    if (!gl) alert( "WebGL isn't available" );

    gl.viewport(0, 0, canvas.width, canvas.height);
    gl.clearColor(0.0, 0.0, 0.0, 1.0);
    
    gl.enable(gl.DEPTH_TEST);

    program = initShaders(gl, "vertex-shader", "fragment-shader");
    gl.useProgram(program);

	// Cube Buffer Data
    colorCube(texCoordsArray[0], texCoord, cvertices, cupointsArray[0]);
    colorCube(texCoordsArray[1], texCoord, vertices, cupointsArray[1]);

    vBuffer1 = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vBuffer1);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(cupointsArray[0]), gl.STATIC_DRAW);

    vBuffer2 = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, vBuffer2);
    gl.bufferData(gl.ARRAY_BUFFER, flatten(cupointsArray[1]), gl.STATIC_DRAW);
    
    vPosition = gl.getAttribLocation(program, "vPosition");
    gl.vertexAttribPointer(vPosition, 4, gl.FLOAT, false, 0, 0);
    gl.enableVertexAttribArray(vPosition);

    for (var m = 0; m < 1 ; m++)
	{
        tBuffer[m] = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, tBuffer[m]);
        gl.bufferData(gl.ARRAY_BUFFER, flatten(texCoordsArray[m]), gl.STATIC_DRAW);
    }
    
	// Store an image from the HTML side
	image[0] = document.getElementById("texImage");
	image[1] = document.getElementById("texImage2");

	// Load the image with different mapping
    configureTexturetri( 1 );
    configureTexturetri( 0 );
	
	// Spheres Buffer Data
    for (var m = 0; m < 3; m++)
	{
		// sets the planet complexity and whether it is flat shaded or not
		//tetrahedron(va, vb, vc, vd, 4, 1, 1);
		// create seperate normal and vertex buffer for each object in the solar system
        nBufferArray[m] = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, nBufferArray[m]);
        gl.bufferData(gl.ARRAY_BUFFER, flatten(normalsArray[m]), gl.STATIC_DRAW);
		
        vBufferArray[m] = gl.createBuffer();
        gl.bindBuffer(gl.ARRAY_BUFFER, vBufferArray[m]);
        gl.bufferData(gl.ARRAY_BUFFER, flatten(pointsArray[m]), gl.STATIC_DRAW);
    }

    modelViewMatrixLoc 	= gl.getUniformLocation(program, "modelViewMatrix");
    projectionMatrixLoc = gl.getUniformLocation(program, "projectionMatrix");
    cameraMatrixLoc 	= gl.getUniformLocation(program, "cameraMatrix");

	
	
    window.addEventListener("keydown",function(event) {
        switch (event.keyCode) {
            case 87:    //W
			if (z[0] > -30) {
                z[0] -= 10;
				for(var i = 1; i < c.length; i++)
				{
					if(c[i] == 1) z[i] -= 10;
				}
			}
                break;
            case 65:     // A
			if (x[0] > -20) {
                x[0] -= 5; 
				for(var i = 1; i < c.length; i++)
				{
					if(c[i] == 1) x[i] -= 5;
				}
			}
                break;
            case 83:    //S
			if (z[0] < 0) {
                z[0] += 10; 
				for(var i = 1; i < c.length; i++)
				{
					if(c[i] == 1) z[i] += 10;
				}
			}
                break;
            case 68:    //D
			if (x[0] < 20) {
                x[0] += 5;
				for(var i = 1; i < c.length; i++)
				{
					if(c[i] == 1) x[i] += 5;
				}
			}
                break;
            case 40: // up
                cameraud += 0.25;
                break;
            case 38: //down
                cameraud += -0.25;
                break;
            case 82: //reset 'r'
                fwdback = -30;
                cameralr = 0;
                //cameraud = 0;
                fovy = 45;
                far =1000;
                near =0.1;
                turn = 0;
                break;
            case 37: // rotate left
                if (turn <= 30)
                turn += 2;
                break;
            case 39: //rotate right
                if (turn >= -30)
                turn -= 2;
                break;
        }
    });
	
    render();
}

///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//////////////////             render              ////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////

var collided = 0;
var modelViewTransforms = [];
var modelViewStack = [];

function initObjs()
{
	x.push(coneX);
	y.push(coneY);
	z.push(coneZ);
	c.push(1);
	p.push(0);
	
	for(var i = 1; i < numScoops+1; i++)
	{
		materialdif.push(colors[getRandomInt(0,6)]);
		x.push(5*getRandomInt(-4,4));
		//y.push(15+5);
		y.push(getRandomInt(10, 40)*2);
		z.push(10*getRandomInt(-3,1));
		c.push(0);
		p.push(i);
	}
}

var heightCount = 16;
function render()
{
	
	resize(gl);
	
    gl.clear( gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
	
    projectionMatrix = perspective(fovy, canvas.width/canvas.height, near, far);
    gl.uniformMatrix4fv(projectionMatrixLoc, false, flatten(projectionMatrix));

    cameraMatrix = mat4();
    cameraMatrix = mult(cameraMatrix, translate (cameralr, cameraud, fwdback));
    cameraMatrix = mult(cameraMatrix, rotate(turn, vec3(0,1,0)));
    gl.uniformMatrix4fv(cameraMatrixLoc, false, flatten(cameraMatrix));
	
	
	var modelViewStack = [x[0],y[0],z[0]];
	var topOfStack = 
	[
	modelViewStack[modelViewStack.length-3],
	modelViewStack[modelViewStack.length-2],
	modelViewStack[modelViewStack.length-1]
	];
	
	
	modelViewTransforms = [];
	for(var i = 1; i < numScoops + 1; i++)
	{
		modelViewTransforms.push(x[i]);
		modelViewTransforms.push(y[i]);
		modelViewTransforms.push(z[i]);
		modelViewTransforms.push(i);
	}
	
    for (var q = 0; q < modelViewTransforms.length ; q += 4)
	{
		collided = 0;
        T = mult(	inverse(translate(topOfStack[0],topOfStack[1],topOfStack[2])),
					translate(modelViewTransforms[q],modelViewTransforms[q+1],modelViewTransforms[q+2]));
		for (var h = 0; h < indices[2]; h++)
		{
			var arraypoint = pointsArray[2];
			var modifiedspherepoint = mult_vec(T, arraypoint[h]);
			if (modifiedspherepoint[0]* modifiedspherepoint[0] +
				modifiedspherepoint[1]* modifiedspherepoint[1] +
				modifiedspherepoint[2]* modifiedspherepoint[2] < 1)
			{
				modelViewTransforms[q] = x[0];
				modelViewTransforms[q+1] = topOfStack[1]+1.5;
				modelViewTransforms[q+2] = z[0];
				c[modelViewTransforms[q+3]] = 1; 		// how  does this work
				p[modelViewTransforms[q+3]] =p[0];
				
				modelViewStack.push(modelViewTransforms[q]);
				modelViewStack.push(modelViewTransforms[q+1]);
				modelViewStack.push(modelViewTransforms[q+2]);
				if(modelViewStack.length > heightCount)
				{
					cameraud -= 7;
					heightCount += 16;
				}
				
				topOfStack = 
				[
				modelViewStack[modelViewStack.length-3],
				modelViewStack[modelViewStack.length-2],
				modelViewStack[modelViewStack.length-1]
				];
				
				modelViewTransforms.splice(q,4);
				collided = 1;
				q = -4;
			}
			if(collided == 1) break;
		}
	}

	enableCube(tBuffer[0], vBuffer1, 1);
	modelViewMatrix = mat4();
    modelViewMatrix = mult(modelViewMatrix, translate(modelViewStack[0],modelViewStack[1],modelViewStack[2]));
	modelViewMatrix = mult(modelViewMatrix, scale(0.7,0.7,0.7));
    gl.uniformMatrix4fv(modelViewMatrixLoc, false, flatten(modelViewMatrix));
    gl.drawArrays(gl.TRIANGLES, 0, numVertices);

    
    enableCube(tBuffer[0], vBuffer2, 0);
    modelViewMatrix = mat4();
    modelViewMatrix = mult(modelViewMatrix, scale(150,150,150));
	modelViewMatrix = mult(modelViewMatrix, rotate(180, vec3(0,1,0)));
    gl.uniformMatrix4fv(modelViewMatrixLoc, false, flatten(modelViewMatrix));
    gl.drawArrays(gl.TRIANGLES, 0, numVertices);
	
	for(var j = 3; j < modelViewStack.length; j += 3)
	{
		bindandlighting(1,j/3);
        modelViewMatrix = mat4();
        modelViewMatrix = mult(modelViewMatrix, translate(modelViewStack[j],modelViewStack[j+1],modelViewStack[j+2]));
        gl.uniformMatrix4fv(modelViewMatrixLoc, false, flatten(modelViewMatrix));
		
        for(var i = 0; i < indices[1]; i += 3)
		{
            gl.drawArrays(gl.TRIANGLES, i, 3);
		}
	}
 
	for(var j = 0; j < modelViewTransforms.length; j += 4)
	{
		bindandlighting(1,j/4);
        modelViewMatrix = mat4();
        modelViewMatrix = mult(modelViewMatrix,translate(modelViewTransforms[j],modelViewTransforms[j+1],modelViewTransforms[j+2]));
        gl.uniformMatrix4fv(modelViewMatrixLoc, false, flatten(modelViewMatrix));

        for(var i = 0; i < indices[1]; i += 3)
		{
            gl.drawArrays(gl.TRIANGLES, i, 3);
		}
	}
	
	for(var i = 1; i < c.length; i++)
	{
		if(c[i] == 0)
		{
			y[i] -= 0.1;
		}
		if(y[i] <= -25)
		{
			x[i] = 5*getRandomInt(-4,4);
			y[i] = 40;
			z[i] = 10*getRandomInt(-1,1);
		}
	}
	
    window.requestAnimFrame(render);
}
