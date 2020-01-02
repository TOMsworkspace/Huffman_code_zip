#define _CRT_SECURE_NO_WARNINGS 1
#include"haffman.h"
#include<iostream>
#include<vector>
#include<assert.h>
#include<string.h>

using namespace std;

typedef long long LongType;
const int max_r = 1024;

struct CharInfo
{
    char _ch;//�ַ�
    LongType _count;//����
    string  _code;//����

    bool operator !=(const CharInfo&info)
    {
        return _count != info._count;
    }
    CharInfo operator+(const CharInfo&info)
    {
        CharInfo ret;
        ret._count = _count + info._count;
        return ret;
    }
    bool operator<(const CharInfo&info)
    {
        return _count > info._count;
    }
};

class FileCompress
{
    typedef HuffmanTreeNode<CharInfo> Node;
    struct TmpInfo
    {
        char _ch;//�ַ�
        LongType _count;//����
    };
public:
    //���캯��
    FileCompress()
    {
        for (size_t i = 0; i < 256; ++i)
        {
            _infos[i]._ch = i;
            _infos[i]._count = 0;
        }
    }
    //��ȡ����������
    void GenerateHuffmanCode(Node*root,string code)//code���ܴ�����??
    {
        if (root == NULL)
            return;
        //ǰ��������ɱ���
        if (root->_pLeft == NULL&&root->_pRight == NULL)
        {
            _infos[(unsigned char )root->_weight._ch]._code = code;
            return;
        }
        GenerateHuffmanCode(root->_pLeft, code+'0');
        GenerateHuffmanCode(root->_pRight, code + '1');
    }
    void Compress(const char *file)//file:Դ�ļ�
    {
        //1.ͳ���ַ����ֵĴ���
        FILE*fout = fopen(file, "rb");
        assert(fout);
        char ch = fgetc(fout);
        while (ch != EOF||feof(fout)==0)//���ļ��������򷵻�ֵΪ1������Ϊ0
        {
            _infos[(unsigned char)ch]._count++;
            ch = fgetc(fout);
        }
        //2.����Huffmantree ��code
        CharInfo invalid;
        invalid._count = 0;
        HuffmanTree<CharInfo>tree(_infos, 256, invalid);//���������飬256������Чֵ������0�Σ�

        string compressfile = file;//
        compressfile += ".huffman";//
        FILE*fin = fopen(compressfile.c_str(),"wb");//��ѹ���ļ�
        assert(fin);


        string code;
        GenerateHuffmanCode(tree.GetRoot(), code);

        //3.0д���ַ����ֵ���Ϣ
        //fwrite(_infos, sizeof(CharInfo), 256, fin);
        int writeNum = 0;
        int objSize = sizeof(TmpInfo);
        for (rsize_t i = 0; i < 256; ++i)
        {
            if (_infos[i]._count>0)
            {
                TmpInfo info;
                info._ch = _infos[i]._ch;
                info._count = _infos[i]._count;
                fwrite(&info, objSize, 1, fin);
                writeNum++;
            }
        }
        TmpInfo info;
        info._count = -1;
        fwrite(&info, objSize, 1, fin);//��info._count = -1д��ȥ��Ϊ������־λ

        //4.ѹ��
        fseek(fout, 0, SEEK_SET);//�ļ�ָ�롢ƫ����������λ��
            ch = fgetc(fout);
            char value = 0;
            size_t pos = 0;
            while (ch != EOF)
            {
                string &code = _infos[(unsigned char)ch]._code;
                for (size_t i = 0; i < code.size(); ++i)
                {
                    if (code[i] == '1')
                        value |= (1<<pos);
                    else if (code[i] == '0')
                    {
                        value &= ~(1<<pos);
                    }
                    else
                    {
                        assert(false);
                    }
                    ++pos; 
                    if (pos == 8)
                    {
                        fputc(value, fin);
                        value = 0;
                        pos = 0;
                    }

                }
                ch = fgetc(fout);
            }
            if (pos > 0)
            {
                fputc(value, fin);//д��ѹ���ļ���fin��
            }
            fclose(fout);
            fclose(fin);
    }
    void uncompress(const char *file)
    {
        string uncompressfile = file;//file:Input.txt.huffman
        size_t pos = uncompressfile.rfind('.');//�ҵ�������һ��'.'
        assert(pos != string::npos);
        uncompressfile.erase(pos);//ɾ����'.'�����ַ���
        uncompressfile = "unhuffman_"+uncompressfile;//Input.txt+'.unhuffman'
        FILE*fin = fopen(uncompressfile.c_str(), "wb");//�򿪽�ѹ���ļ�
        assert(fin);
        FILE*fout = fopen(file, "rb");//��ѹ���ļ�
        assert(fout);
        //fread(_infos, sizeof(CharInfo), 256, fout);
        //3.0�����ַ����ֵ���Ϣ
        TmpInfo info;
        int cycleNum = 1;
        int objSize = sizeof(TmpInfo);
        fread(&info, objSize, 1, fout);

        while (info._count != -1)//-1Ϊ������־
        {
            _infos[(unsigned char)info._ch]._ch = info._ch;
            _infos[(unsigned char)info._ch]._count= info._count;

            fread(&info, objSize, 1, fout);
            cycleNum++;
        }

        int aaa = 0;
        //�ؽ�huaffman��
        CharInfo invalid;
        invalid._count = 0;
        HuffmanTree<CharInfo>tree(_infos, 256, invalid);//���������飬256������Чֵ������0�Σ�
        Node *root = tree.GetRoot();
        Node*cur = root;
        LongType n = root->_weight._count;//����Ҷ�ӽڵ�ĺͣ�Դ�ļ��ַ��ĸ�����
        char ch = fgetc(fout);//��fout(ѹ���ļ�)���ַ�
        while (ch != EOF||n>0)
        {
            for (size_t i = 0; i < 8; ++i)
            {

                if ((ch&(1 << i)) == 0)
                    cur = cur->_pLeft;
                else
                    cur = cur->_pRight;
                if (cur->_pLeft == NULL&&cur->_pRight == NULL)
                {
                    //cout << cur->_weight._ch;
                    fputc(cur->_weight._ch, fin);//fin��ѹ���ļ�
                    cur = root;
                    if (--n == 0)
                        break;
                }

            }
            ch = fgetc(fout);
        }
        fclose(fin);
        fclose(fout);

    }
    
    
    
    // Calculate the file size
void Get_file_size(char *name_1, char *name_2)
{
    FILE *fp_1 = fopen(name_1, "r");
    FILE *fp_2 = fopen(name_2, "r");
    if (fp_1)
    {
        printf ("Size of %s : ", name_1);
        fseek(fp_1, 0,  SEEK_END);
        if (1.*ftell(fp_1)/(1024*1024*1024) > 1.0)
               printf("%.1f GB\n", 1.*ftell(fp_1)/(1024*1024*1024));
           else if(1.*ftell(fp_1)/(1024*1024) > 1.0)
               printf("%.1f MB\n", 1.*ftell(fp_1)/(1024*1024));
           else if (1.*ftell(fp_1)/(1024) > 1.0)
               printf("%.1f KB\n", 1.*ftell(fp_1)/(1024));
           else printf ("%d Bt\n", ftell(fp_1));
        fclose(fp_1);
    }
    if (fp_2)
    {
        printf ("Size of %s : ", name_2);
        fseek(fp_2, 0,  SEEK_END);
        if (1.*ftell(fp_2)/(1024*1024*1024) > 1.0)
               printf("%.1f GB\n", 1.*ftell(fp_2)/(1024*1024*1024));
           else if(1.*ftell(fp_2)/(1024*1024) > 1.0)
               printf("%.1f MB\n", 1.*ftell(fp_2)/(1024*1024));
           else if (1.*ftell(fp_2)/(1024) > 1.0)
               printf("%.1f KB\n", 1.*ftell(fp_2)/(1024));
           else printf ("%d Bt\n", ftell(fp_2));
        fclose(fp_2);
    }
    return;
}

//�Ƚ������ļ��Ƿ���ͬ 
void compare(char* name_1,char* name_2)
{
    int AC, line_1, line_2;
    char ch_1[max_r], ch_2[max_r];
    
    AC = 1;
    line_1 = line_2 = 0;
    Get_file_size(name_1, name_2);
    
    FILE *fp_1 = fopen(name_1, "r");
    FILE *fp_2 = fopen(name_2, "r");
    while(1)
    {
        char *end_1, *end_2;
        end_1 = fgets(ch_1, max_r, fp_1);
        end_2 = fgets(ch_2, max_r, fp_2);
        if (end_1 != NULL)
            line_1 ++;
        if (end_2 != NULL)
            line_2 ++;
        while(strcmp(ch_1, "\n") == 0 && end_1 != NULL)
        {
            end_1 = fgets(ch_1, max_r, fp_1);
            if (end_1 != NULL)
                line_1 ++;
        }
        while(strcmp(ch_2, "\n") == 0 && end_2 != NULL)
        {
            end_2 = fgets(ch_2, max_r, fp_2);
            if (end_2 != NULL)
                line_2 ++;
        }
        if (end_1 == NULL && end_2 == NULL)
            break;
        if(strcmp(ch_1, ch_2) != 0)
        {
            AC = 0;
            ch_1[strlen(ch_1) -1] = '\0';
            ch_2[strlen(ch_2) -1] = '\0';
            printf ("    Worry Answer\n");
            printf ("In file %s at %d line << %s >> difference from\n", name_1, line_1, ch_1);
            printf ("In file %s at %d line << %s >>\n", name_2, line_2, ch_2);
            break;
        }
    }
    if (AC)
    {
        if (line_1 != line_2)
        {
            printf ("    Print Error\n");
            printf ("The print not in a canonical format!\n");
            printf ("the %s have %d lines \nbut %s have %d lines!\n", name_1, line_1, name_2, line_2);
        }
        else
            printf ("    Aceept\n", line_1, line_2);
    }
    fclose(fp_1);
    fclose(fp_2);
}

protected:
    CharInfo _infos[256];
};

int main(void)
{

    FileCompress fc;
    FileCompress fc1;
    //fc.Compress("s.txt");
    //fc1.uncompress("s.txt.huffman");

    //fc.Compress("Input.txt");
    //fc1.uncompress("Input.txt.huffman");

    fc.Compress("plaintext.docx");
    fc1.uncompress("plaintext.docx.huffman");
    
    fc1.compare("plaintext.docx","unhuffman_plaintext.docx");

    //fc.Compress("zhizhen.doc");
    //fc1.uncompress("zhizhen.doc.huffman");
    return 0;
}

