#include "ObjReader.h"

//初始缓冲大小。
UINT ObjReader::initVectorSize = 100;

ObjReader::ObjReader()
{
}


ObjReader::~ObjReader()
{
}

unique_ptr<unordered_map<
	string, unique_ptr<GeometryGenerator::MeshData>>> ObjReader::ReadObjFile(string filePath)
{
	ifstream fileStream(filePath);
	if ( ! fileStream.is_open())
	{
		string outputString;
		outputString = "读取文件失败，请检查文件：" + filePath;
		throw SimpleException(outputString.c_str(), __FILE__, __LINE__);
	}

	//顶点坐标。
	vector<XMFLOAT3> vertextLocations;
	//顶点贴图坐标。
	vector<XMFLOAT3> textureLocations;
	//顶点法线，因为一般来讲法线的定义比顶点坐标要多，所以这里的vector多分配一些空间。
	vector<XMFLOAT3> normals;
	//单个物体的定义，这里面包括这个物体所有三角面的索引。
	vector<unique_ptr<ObjectGeometry>> Geometrys;

	//读取关键数据，因为obj文件中的数据是全局的，所以先整个读取，之后再转换。
	ReadKeyInfo(
		fileStream,					//目标文件流
		vertextLocations,		//所有顶点坐标定义
		textureLocations,		//贴图坐标定义。
		normals,				//法线方向定义。
		Geometrys);			//所有几何体的定义。

	//关闭文件。
	fileStream.close();
	
	//整理所有读取的数据，转换为MeshData，并且每一个单独的子网格放到一个MeshData中。
	return Convert(
		vertextLocations,		//所有顶点坐标定义
		textureLocations,		//贴图坐标定义。
		normals,				//法线方向定义。
		Geometrys);			//所有几何体的定义。
}

void ObjReader::ReadKeyInfo(ifstream & openedFile, vector<XMFLOAT3>& vs, 
	vector<XMFLOAT3>& vts, vector<XMFLOAT3>& ns, vector<unique_ptr<ObjectGeometry>>& geos)
{
	string lineBuffer;
	
	//读到文件末尾。
	while (!openedFile.eof())
	{
		//读取一行。
		getline(openedFile, lineBuffer);

		//开始分析行，这个标记的主要作用是因为添加面定义的时候回跳到另一个函数中执行，
		//解释面定义的时候回多读取一行字符串，
		//为了防止这个多的一行字符串包含的有用信息被忽略，
		//跳出面定义读取之后立马跳转到这个语句重新分析字符串。
		startAnalyzeLine:

		//查看第一个字符。
		switch (lineBuffer[0])
		{
			//顶点坐标、贴图、法线。
		case 'v':
			//进一步检查第二个字符。
			switch (lineBuffer[1])
			{
				//顶点坐标
			case ' ':
				//存储顶点坐标。
				SaveVertexLocation(lineBuffer, vs);
				break;

				//贴图坐标
			case 't':
				SaveTextureLocation(lineBuffer, vts);
				break;

				//法线向量
			case 'n':
				SaveNormal(lineBuffer, ns);
				break;

			default:
				break;
			}
			break;

			//单个几何体。
		case 'g':
			//读取一个几何体的面信息，
			//当读取完毕之后，返回下一行的字符串到lineBuffer中。
			ReadKeyInfo_For_OneGeometry(openedFile, lineBuffer, geos);
			goto startAnalyzeLine;
			break;

		default:
			break;
		}
	}
}

unique_ptr<unordered_map<string, unique_ptr<GeometryGenerator::MeshData>>> ObjReader::Convert(
	vector<XMFLOAT3>& vs, vector<XMFLOAT3>& vts, 
	vector<XMFLOAT3>& ns, vector<unique_ptr<ObjectGeometry>>& geos)
{
	auto organizedMap = 
		std::make_unique<
		unordered_map<
			string, 
			unique_ptr<GeometryGenerator::MeshData>>>();

	for (auto& geo : geos)
	{
		auto newMesh = std::make_unique<GeometryGenerator::MeshData>();

		for (int i = 0; i < geo->faceCount; ++i)
		{
			//各个顶点的索引。
			UINT index_v1 = i * 3 + 0;
			UINT index_v2 = i * 3 + 1;
			UINT index_v3 = i * 3 + 2;

			//各个顶点的要求的数据索引。
			UINT	
				//顶点1
				index_v1_p	= geo->LocationList	[index_v1] - 1,	//坐标索引
				index_v1_n	= geo->NormalList	[index_v1] - 1,	//法线索引
				index_v1_t	= geo->TextureList	[index_v1] - 1,	//贴图坐标索引
				//顶点2
				index_v2_p	= geo->LocationList	[index_v2] - 1,
				index_v2_n	= geo->NormalList	[index_v2] - 1,
				index_v2_t	= geo->TextureList	[index_v2] - 1,
				//顶点3
				index_v3_p	= geo->LocationList	[index_v3] - 1,
				index_v3_n	= geo->NormalList	[index_v3] - 1,
				index_v3_t	= geo->TextureList	[index_v3] - 1;

			/*
			XMFLOAT2
			v1_texture(GET_X_Y_ARGS(vts[index_v1_t])),
			v2_texture(GET_X_Y_ARGS(vts[index_v2_t])),
			v3_texture(GET_X_Y_ARGS(vts[index_v3_t]));
			*/
			
			//添加顶点定义，为了简化，这里不考虑顶点的切向量。
			GeometryGenerator::Vertex v1(
				GET_X_Y_Z_ARGS(vs[index_v1_p]),		//坐标。
				GET_X_Y_Z_ARGS(ns[index_v1_n]),		//法线。
				0.0f, 0.0f, 0.0f,					//切线向量。
				GET_X_Y_ARGS(vts[index_v1_t]));		//贴图坐标。
			GeometryGenerator::Vertex v2(
				GET_X_Y_Z_ARGS(vs[index_v2_p]),		//坐标。
				GET_X_Y_Z_ARGS(ns[index_v2_n]),		//法线。
				0.0f, 0.0f, 0.0f,					//切线向量。
				GET_X_Y_ARGS(vts[index_v2_t]));		//贴图坐标。
			GeometryGenerator::Vertex v3(
				GET_X_Y_Z_ARGS(vs[index_v3_p]),		//坐标。
				GET_X_Y_Z_ARGS(ns[index_v3_n]),		//法线。
				0.0f, 0.0f, 0.0f,					//切线向量。
				GET_X_Y_ARGS(vts[index_v3_t]));		//贴图坐标。

//***************** 缠 绕 方 向 改 变 ****************************************************************
//***************** 缠 绕 方 向 改 变 ****************************************************************
			newMesh->Vertices.push_back(v1);														//
			newMesh->Vertices.push_back(v3);														//
			newMesh->Vertices.push_back(v2);														//
//***************** 缠 绕 方 向 改 变 ****************************************************************
//***************** 缠 绕 方 向 改 变 ****************************************************************

			//添加顶点索引。
			newMesh->Indices32.push_back(static_cast<uint32>(index_v1));
			newMesh->Indices32.push_back(static_cast<uint32>(index_v2));
			newMesh->Indices32.push_back(static_cast<uint32>(index_v3));
		}

		//添加几何体网格数据。
		(*organizedMap)[geo->name] = std::move(newMesh);
	}

	return organizedMap;
}

void ObjReader::SaveVertexLocation(string & line, vector<XMFLOAT3>& vs)
{
	vector<string> subStrings;
	//以空格分割字符串，注意表示顶点的这一行类似：“v  -3.2065 1.9203 -7.3193”
	//其中v后面有两个空格，这会使得分割出来的字符串有一个空的，注意跳过。
	SplitString(line, subStrings, " ");

	//有的obj文件，v后面之后一个空格，为了避免这种情况，
	//固定从子字符串的后三个开始提取。
	int size = subStrings.size();

	//X轴取负值、Z轴取Y轴的负值、Y轴取Z轴。
	vs.push_back(
		XMFLOAT3( 
				UNIT_CONVERT(strtof(subStrings[size - 3].c_str(), nullptr)),
				UNIT_CONVERT(strtof(subStrings[size - 2].c_str(), nullptr)),
			   -UNIT_CONVERT(strtof(subStrings[size - 1].c_str(), nullptr))	//这里要取负值
			)
		);
}

void ObjReader::SaveTextureLocation(string & line, vector<XMFLOAT3>& vts)
{
	vector<string> subStrings;
	//以空格分割字符串，注意表示顶点的这一行类似：“vt -3.2065 1.9203 -7.3193”。
	SplitString(line, subStrings, " ");

	vts.push_back(
		XMFLOAT3(
			strtof(subStrings[1].c_str(), nullptr),
			strtof(subStrings[2].c_str(), nullptr),
			1.0f	//有的obj文件vt后面有三个数（第三个固定为1），而有的只有两个数，
		)
	);
}

void ObjReader::SaveNormal(string & line, vector<XMFLOAT3>& ns)
{
	vector<string> subStrings;
	//以空格分割字符串，注意表示顶点的这一行类似：“vn -3.2065 1.9203 -7.3193”。
	SplitString(line, subStrings, " ");

	ns.push_back(
		XMFLOAT3(
			strtof(subStrings[1].c_str(), nullptr), 
			strtof(subStrings[2].c_str(), nullptr),
			-strtof(subStrings[3].c_str(), nullptr)
		)
	);
}

void ObjReader::ReadKeyInfo_For_OneGeometry(ifstream & openedFile, string & line, vector<unique_ptr<ObjectGeometry>>& geos)
{
	string lineBuffer;
	vector<string> subStrings;

	//从初始行获取几何体的名字，第一行的形式类似：“g Box002”
	SplitString(line, subStrings, " ");
	//设置名字。
	auto newGeo = std::make_unique<ObjectGeometry>(subStrings[1]);

	do
	{
		//读取行，知道#开头的字符串停止读取。
		getline(openedFile, lineBuffer);
		if (lineBuffer[0] == 'f')
		{
			//添加一个三角面的定义。
			newGeo->AddFaceDefination(lineBuffer);
		}
		else
		{
			break;
		}

	} while (lineBuffer[0] == 'f' && !openedFile.eof());

	//添加新几何体。
	geos.push_back(std::move(newGeo));


	//跳出循环，为了防止最后读取的一个不是三角面定义的字符串是有用信息，
	//将引用参数line设为最后读取的一个字符串，方便上层调用者对这一行信息重新分析，
	//防止遗漏有用信息。
	line = lineBuffer;
}

ObjReader::ObjectGeometry::ObjectGeometry(const string & _name)
	:name(_name), faceCount(0)
{
}

void ObjReader::ObjectGeometry::AddFaceDefination(const string& faceDefination)
{
	vector<string> subStrings;
	SplitString(faceDefination, subStrings, " ");

	//一次添加三个顶点定义，组成一个面。
	AddVertexDefination(subStrings[1]);
	AddVertexDefination(subStrings[2]);
	AddVertexDefination(subStrings[3]);

	//面数记录加一。
	++faceCount;
}

void ObjReader::ObjectGeometry::AddVertexDefination(const string & vertextDefination)
{
	vector<string> subNumber;
	SplitString(vertextDefination, subNumber, "/");

	//顶点位置索引。
	LocationList.push_back( static_cast<UINT>(
			_strtoui64( subNumber[0].c_str(), nullptr, 10)));

	//顶点贴图坐标索引。
	TextureList.push_back(static_cast<UINT>(
		_strtoui64(subNumber[1].c_str(), nullptr, 10)));

	//顶点法线坐标索引。
	NormalList.push_back(static_cast<UINT>(
		_strtoui64(subNumber[2].c_str(), nullptr, 10)));
}
