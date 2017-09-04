#pragma once
#include "../../../../Common/GeometryGenerator.h"
#include "MyTools.h"
using namespace DirectX; 
using namespace std;

typedef unsigned int UINT;

#define GET_X_Y_Z_ARGS(float3) float3.x, float3.y, float3.z
#define GET_X_Y_ARGS(float2) float2.x, float2.y

//将英寸单位转换为厘米单位，乘以2.54。
#define INCH_TO_CM(floatNumberInInch) (floatNumberInInch * 2.54f)

//将英寸单位转换为米单位，乘以0.0254。
#define INCH_TO_M(floatNumberInInch) (floatNumberInInch * 0.0254f)

//使用哪一种单位。
//厘米
#define USE_CM_AS_UNIT false
//米
#define USE_M_AS_UNIT true

//选择进制转换。
#if(USE_CM_AS_UNIT)
	#define UNIT_CONVERT INCH_TO_CM
#elif(USE_M_AS_UNIT)
	#define UNIT_CONVERT INCH_TO_M
#else
	#define UNIT_CONVERT ASSERT(false && "Complie Error! Please select one Unit standard to read file.obj");
#endif

class ObjReader
{
public:
	ObjReader();
	~ObjReader();

	using uint16 = std::uint16_t;
	using uint32 = std::uint32_t;

	//Obj文件中，单个子网格的面定义，
	//每个面三个顶点，
	//每个顶点包含顶点的坐标、贴图、法线的信息在这个Obj文件中的索引。
	struct ObjectGeometry
	{
		//网格的名字。
		string name;
		//顶点坐标。
		 vector<UINT> LocationList;
		//贴图坐标。
		vector<UINT> TextureList;
		//法线。
		vector<UINT> NormalList;
		//面数。
		UINT faceCount;

	public:
		ObjectGeometry(const string& _name);
		//形如“f 91/12/862 94/15/863 102/15/864”这样的三角面定义，
		//添加各个索引值。
		void AddFaceDefination(const string& faceDefination);
	protected:
		//添加顶点定义，要求最好一次添加三个顶点，组成一个面。
		void AddVertexDefination(const string& vertextDefination);
	};

public:
	//初始化缓冲的大小，用于构建绝大多数Vector的初始大小。
	static UINT initVectorSize;

	//读取指定的Obj文件，将这个文件中的所有单个Obj读取为一个unorderedMap，
	//每一个名字对应一个物体网格。
	static unique_ptr<unordered_map< 
		string, unique_ptr<GeometryGenerator::MeshData>>> ReadObjFile(string filePath);

protected:
	//从obj文件中读取关键数据到各个vector中。等待后续的处理，不负责关闭文件。
	static void ReadKeyInfo(
		ifstream& openedFile,		//已经打开的Obj文件流，不负责关上。
		vector<XMFLOAT3>& vs,		//顶点坐标缓冲。
		vector<XMFLOAT3>& vts,		//贴图坐标缓冲。
		vector<XMFLOAT3>& ns,		//法线向量缓冲。
		vector<unique_ptr<ObjectGeometry>>& geos);	//单个子网格定义，以及每个面的索引。

	//将obj文件中读取的关键数据进行转化，变成MeshData。
	static unique_ptr<unordered_map<
		string, unique_ptr<GeometryGenerator::MeshData>>> Convert(
			vector<XMFLOAT3>& vs,		//顶点坐标缓冲。
			vector<XMFLOAT3>& vts,		//贴图坐标缓冲。
			vector<XMFLOAT3>& ns,		//法线向量缓冲。
			vector<unique_ptr<ObjectGeometry>>& geos);	//单个子网格定义，以及每个面的索引。

	//从指定的字符串中提取顶点的坐标信息。
	static void SaveVertexLocation(string& line, vector<XMFLOAT3>& vs);
	//从指定的字符串中提取顶点的贴图坐标。
	static void SaveTextureLocation(string& line, vector<XMFLOAT3>& vts);
	//从指定的字符串中提取顶点的法线向量。
	static void SaveNormal(string& line, vector<XMFLOAT3>& ns);

	//已知当前文件读取的行数是g打头，表示一个几何体，当前行的信息是line，
	//从这个文件接下来的内容中提取这个几何体的面信息，
	//当前几何体的面信息一旦终止，便终止文件读取，
	//不负责关闭文件。
	static void ReadKeyInfo_For_OneGeometry(ifstream& openedFile, string& line, vector<unique_ptr<ObjectGeometry>>& geos);
};
