#pragma once
#include "../../../../Common/GeometryGenerator.h"
#include "MyTools.h"
using namespace DirectX; 
using namespace std;

typedef unsigned int UINT;

#define GET_X_Y_Z_ARGS(float3) float3.x, float3.y, float3.z
#define GET_X_Y_ARGS(float2) float2.x, float2.y

//��Ӣ�絥λת��Ϊ���׵�λ������2.54��
#define INCH_TO_CM(floatNumberInInch) (floatNumberInInch * 2.54f)

//��Ӣ�絥λת��Ϊ�׵�λ������0.0254��
#define INCH_TO_M(floatNumberInInch) (floatNumberInInch * 0.0254f)

//ʹ����һ�ֵ�λ��
//����
#define USE_CM_AS_UNIT false
//��
#define USE_M_AS_UNIT true

//ѡ�����ת����
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

	//Obj�ļ��У�������������涨�壬
	//ÿ�����������㣬
	//ÿ�����������������ꡢ��ͼ�����ߵ���Ϣ�����Obj�ļ��е�������
	struct ObjectGeometry
	{
		//��������֡�
		string name;
		//�������ꡣ
		 vector<UINT> LocationList;
		//��ͼ���ꡣ
		vector<UINT> TextureList;
		//���ߡ�
		vector<UINT> NormalList;
		//������
		UINT faceCount;

	public:
		ObjectGeometry(const string& _name);
		//���硰f 91/12/862 94/15/863 102/15/864�������������涨�壬
		//��Ӹ�������ֵ��
		void AddFaceDefination(const string& faceDefination);
	protected:
		//��Ӷ��㶨�壬Ҫ�����һ������������㣬���һ���档
		void AddVertexDefination(const string& vertextDefination);
	};

public:
	//��ʼ������Ĵ�С�����ڹ����������Vector�ĳ�ʼ��С��
	static UINT initVectorSize;

	//��ȡָ����Obj�ļ���������ļ��е����е���Obj��ȡΪһ��unorderedMap��
	//ÿһ�����ֶ�Ӧһ����������
	static unique_ptr<unordered_map< 
		string, unique_ptr<GeometryGenerator::MeshData>>> ReadObjFile(string filePath);

protected:
	//��obj�ļ��ж�ȡ�ؼ����ݵ�����vector�С��ȴ������Ĵ���������ر��ļ���
	static void ReadKeyInfo(
		ifstream& openedFile,		//�Ѿ��򿪵�Obj�ļ�������������ϡ�
		vector<XMFLOAT3>& vs,		//�������껺�塣
		vector<XMFLOAT3>& vts,		//��ͼ���껺�塣
		vector<XMFLOAT3>& ns,		//�����������塣
		vector<unique_ptr<ObjectGeometry>>& geos);	//�����������壬�Լ�ÿ�����������

	//��obj�ļ��ж�ȡ�Ĺؼ����ݽ���ת�������MeshData��
	static unique_ptr<unordered_map<
		string, unique_ptr<GeometryGenerator::MeshData>>> Convert(
			vector<XMFLOAT3>& vs,		//�������껺�塣
			vector<XMFLOAT3>& vts,		//��ͼ���껺�塣
			vector<XMFLOAT3>& ns,		//�����������塣
			vector<unique_ptr<ObjectGeometry>>& geos);	//�����������壬�Լ�ÿ�����������

	//��ָ�����ַ�������ȡ�����������Ϣ��
	static void SaveVertexLocation(string& line, vector<XMFLOAT3>& vs);
	//��ָ�����ַ�������ȡ�������ͼ���ꡣ
	static void SaveTextureLocation(string& line, vector<XMFLOAT3>& vts);
	//��ָ�����ַ�������ȡ����ķ���������
	static void SaveNormal(string& line, vector<XMFLOAT3>& ns);

	//��֪��ǰ�ļ���ȡ��������g��ͷ����ʾһ�������壬��ǰ�е���Ϣ��line��
	//������ļ�����������������ȡ��������������Ϣ��
	//��ǰ�����������Ϣһ����ֹ������ֹ�ļ���ȡ��
	//������ر��ļ���
	static void ReadKeyInfo_For_OneGeometry(ifstream& openedFile, string& line, vector<unique_ptr<ObjectGeometry>>& geos);
};
