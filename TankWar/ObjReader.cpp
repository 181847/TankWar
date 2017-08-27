#include "ObjReader.h"

//��ʼ�����С��
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
		outputString = "��ȡ�ļ�ʧ�ܣ������ļ���" + filePath;
		throw SimpleException(outputString.c_str(), __FILE__, __LINE__);
	}

	//�������ꡣ
	vector<XMFLOAT3> vertextLocations;
	//������ͼ���ꡣ
	vector<XMFLOAT3> textureLocations;
	//���㷨�ߣ���Ϊһ���������ߵĶ���ȶ�������Ҫ�࣬���������vector�����һЩ�ռ䡣
	vector<XMFLOAT3> normals;
	//��������Ķ��壬���������������������������������
	vector<unique_ptr<ObjectGeometry>> Geometrys;

	//��ȡ�ؼ����ݣ���Ϊobj�ļ��е�������ȫ�ֵģ�������������ȡ��֮����ת����
	ReadKeyInfo(
		fileStream,					//Ŀ���ļ���
		vertextLocations,		//���ж������궨��
		textureLocations,		//��ͼ���궨�塣
		normals,				//���߷����塣
		Geometrys);			//���м�����Ķ��塣

	//�ر��ļ���
	fileStream.close();
	
	//�������ж�ȡ�����ݣ�ת��ΪMeshData������ÿһ��������������ŵ�һ��MeshData�С�
	return Convert(
		vertextLocations,		//���ж������궨��
		textureLocations,		//��ͼ���궨�塣
		normals,				//���߷����塣
		Geometrys);			//���м�����Ķ��塣
}

void ObjReader::ReadKeyInfo(ifstream & openedFile, vector<XMFLOAT3>& vs, 
	vector<XMFLOAT3>& vts, vector<XMFLOAT3>& ns, vector<unique_ptr<ObjectGeometry>>& geos)
{
	string lineBuffer;
	
	//�����ļ�ĩβ��
	while (!openedFile.eof())
	{
		//��ȡһ�С�
		getline(openedFile, lineBuffer);

		//�鿴��һ���ַ���
		switch (lineBuffer[0])
		{
			//�������ꡢ��ͼ�����ߡ�
		case 'v':
			//��һ�����ڶ����ַ���
			switch (lineBuffer[1])
			{
				//��������
			case ' ':
				//�洢�������ꡣ
				SaveVertexLocation(lineBuffer, vs);
				break;

				//��ͼ����
			case 't':
				SaveTextureLocation(lineBuffer, vts);
				break;

				//��������
			case 'n':
				SaveNormal(lineBuffer, ns);
				break;

			default:
				break;
			}
			break;

			//���������塣
		case 'g':
			//��ȡһ�������������Ϣ��
			ReadKeyInfo_For_OneGeometry(openedFile, lineBuffer, geos);
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
			//���������������
			UINT index_v1 = i + 0;
			UINT index_v2 = i + 1;
			UINT index_v3 = i + 2;

			//���������Ҫ�������������
			UINT	
				//����1
				index_v1_p	= geo->LocationList	[index_v1] - 1,	//��������
				index_v1_n	= geo->NormalList	[index_v1] - 1,	//��������
				index_v1_t	= geo->TextureList	[index_v1] - 1,	//��ͼ��������
				//����2
				index_v2_p	= geo->LocationList	[index_v2] - 1,
				index_v2_n	= geo->NormalList	[index_v2] - 1,
				index_v2_t	= geo->TextureList	[index_v2] - 1,
				//����3
				index_v3_p	= geo->LocationList	[index_v3] - 1,
				index_v3_n	= geo->NormalList	[index_v3] - 1,
				index_v3_t	= geo->TextureList	[index_v3] - 1;

			/*
			XMFLOAT2
			v1_texture(GET_X_Y_ARGS(vts[index_v1_t])),
			v2_texture(GET_X_Y_ARGS(vts[index_v2_t])),
			v3_texture(GET_X_Y_ARGS(vts[index_v3_t]));
			*/
			
			//��Ӷ��㶨�壬Ϊ�˼򻯣����ﲻ���Ƕ������������
			GeometryGenerator::Vertex v1(
				GET_X_Y_Z_ARGS(vs[index_v1_p]),		//���ꡣ
				GET_X_Y_Z_ARGS(ns[index_v1_n]),		//���ߡ�
				0.0f, 0.0f, 0.0f,					//����������
				GET_X_Y_ARGS(vts[index_v1_t]));		//��ͼ���ꡣ
			GeometryGenerator::Vertex v2(
				GET_X_Y_Z_ARGS(vs[index_v2_p]),		//���ꡣ
				GET_X_Y_Z_ARGS(ns[index_v2_n]),		//���ߡ�
				0.0f, 0.0f, 0.0f,					//����������
				GET_X_Y_ARGS(vts[index_v2_t]));		//��ͼ���ꡣ
			GeometryGenerator::Vertex v3(
				GET_X_Y_Z_ARGS(vs[index_v3_p]),		//���ꡣ
				GET_X_Y_Z_ARGS(ns[index_v3_n]),		//���ߡ�
				0.0f, 0.0f, 0.0f,					//����������
				GET_X_Y_ARGS(vts[index_v3_t]));		//��ͼ���ꡣ

			newMesh->Vertices.push_back(v1);
			newMesh->Vertices.push_back(v2);
			newMesh->Vertices.push_back(v3);

			//��Ӷ���������
			newMesh->Indices32.push_back(static_cast<uint32>(index_v1));
			newMesh->Indices32.push_back(static_cast<uint32>(index_v2));
			newMesh->Indices32.push_back(static_cast<uint32>(index_v3));
		}

		//��Ӽ������������ݡ�
		(*organizedMap)[geo->name] = std::move(newMesh);
	}

	return organizedMap;
}

void ObjReader::SaveVertexLocation(string & line, vector<XMFLOAT3>& vs)
{
	vector<string> subStrings;
	//�Կո�ָ��ַ�����ע���ʾ�������һ�����ƣ���v  -3.2065 1.9203 -7.3193��
	//����v�����������ո����ʹ�÷ָ�������ַ�����һ���յģ�ע��������
	SplitString(line, subStrings, " ");

	vs.push_back(
		XMFLOAT3( 
			INCH_TO_CM(strtof(subStrings[2].c_str(), nullptr)),
			INCH_TO_CM(strtof(subStrings[3].c_str(), nullptr)),
			INCH_TO_CM(strtof(subStrings[4].c_str(), nullptr))
			)
		);
}

void ObjReader::SaveTextureLocation(string & line, vector<XMFLOAT3>& vts)
{
	vector<string> subStrings;
	//�Կո�ָ��ַ�����ע���ʾ�������һ�����ƣ���vt -3.2065 1.9203 -7.3193����
	SplitString(line, subStrings, " ");

	vts.push_back(
		XMFLOAT3(
			strtof(subStrings[1].c_str(), nullptr),
			strtof(subStrings[2].c_str(), nullptr),
			strtof(subStrings[3].c_str(), nullptr)
		)
	);
}

void ObjReader::SaveNormal(string & line, vector<XMFLOAT3>& ns)
{
	vector<string> subStrings;
	//�Կո�ָ��ַ�����ע���ʾ�������һ�����ƣ���vn -3.2065 1.9203 -7.3193����
	SplitString(line, subStrings, " ");

	ns.push_back(
		XMFLOAT3(
			strtof(subStrings[1].c_str(), nullptr),
			strtof(subStrings[2].c_str(), nullptr),
			strtof(subStrings[3].c_str(), nullptr)
		)
	);
}

void ObjReader::ReadKeyInfo_For_OneGeometry(ifstream & openedFile, string & line, vector<unique_ptr<ObjectGeometry>>& geos)
{
	string lineBuffer;
	vector<string> subStrings;

	//�ӳ�ʼ�л�ȡ����������֣���һ�е���ʽ���ƣ���g Box002��
	SplitString(line, subStrings, " ");
	//�������֡�
	auto newGeo = std::make_unique<ObjectGeometry>(subStrings[1]);

	do
	{
		//��ȡ�У�֪��#��ͷ���ַ���ֹͣ��ȡ��
		getline(openedFile, lineBuffer);
		if (lineBuffer[0] == 'f')
		{
			//���һ��������Ķ��塣
			newGeo->AddFaceDefination(lineBuffer);
		}

	} while (lineBuffer[0] != '#' && !openedFile.eof());

	//����¼����塣
	geos.push_back(std::move(newGeo));
}

ObjReader::ObjectGeometry::ObjectGeometry(const string & _name)
	:name(_name), faceCount(0)
{
}

void ObjReader::ObjectGeometry::AddFaceDefination(const string& faceDefination)
{
	vector<string> subStrings;
	SplitString(faceDefination, subStrings, " ");

	//һ������������㶨�壬���һ���档
	AddVertexDefination(subStrings[1]);
	AddVertexDefination(subStrings[2]);
	AddVertexDefination(subStrings[3]);

	//������¼��һ��
	++faceCount;
}

void ObjReader::ObjectGeometry::AddVertexDefination(const string & vertextDefination)
{
	vector<string> subNumber;
	SplitString(vertextDefination, subNumber, "/");

	//����λ��������
	LocationList.push_back( static_cast<UINT>(
			_strtoui64( subNumber[0].c_str(), nullptr, 10)));

	//������ͼ����������
	TextureList.push_back(static_cast<UINT>(
		_strtoui64(subNumber[1].c_str(), nullptr, 10)));

	//������ͼ����������
	NormalList.push_back(static_cast<UINT>(
		_strtoui64(subNumber[2].c_str(), nullptr, 10)));
}
