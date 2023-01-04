#pragma once
#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <d3d12.h>
#include <DirectXMath.h>
#include <dxgi1_6.h>
#include <string>
#include <vector>

#include "d3dx12.h"
#include "ComPtr.h"
#include "RootSignature.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")


// Core�N���X
// �e�`��I�u�W�F�N�g�ŋ��L�ł�����̂��Ǘ�
class Core
{
public:
	Core();
	virtual ~Core();

	void DebugSummary() const;

	void Initialize(HWND hwnd);
	void Cleanup();
	void Terminate();

	ComPtr<ID3D12Device> GetDevice();
	ComPtr<ID3D12GraphicsCommandList> GetCommandList();
	std::shared_ptr<RootSignature> GetRootSignature(int n);


	const UINT GpuWaitTimeout = (10 * 1000); // 10s
	const UINT FrameBufferCount = 2;
	int width = 0;
	int height = 0;
	DirectX::XMFLOAT4 clearColor = {0.5,0.5,0.5,0.5};
private: // �`��Ɏg�p����DirectX12�̃I�u�W�F�N�g
	// �Ȃ�ׂ����|�C���^���g�킸�X�}�[�g�|�C���^ComPtr���g�p����

	UINT m_dxgiFlags;
	// �f�o�C�X
	ComPtr<ID3D12Device> m_device;

	// �t�@�N�g���[
	// �X���b�v�`�F�C����f�o�C�X���쐬���邽�߂Ɏg�p
	ComPtr<IDXGIFactory3> m_factory;

	// �X���b�v�`�F�[��
	// �t���[���o�b�t�@���X���b�v���邽�߂ɕK�v
	ComPtr<IDXGISwapChain4> m_swapchain;

	// �`��R�}���h�֘A

	// �R�}���h�L���[
	// �R�}���h���X�g�̑҂��s��
	// �R�}���h���X�g�ɕ`��R�}���h�𒙂߂� > �R�}���h�L���[�ɃR�}���h���X�g������
	// > �L���[�ɗ��܂������X�g���������s
	// �̗���
	ComPtr<ID3D12CommandQueue> m_commandQueue;
	// �R�}���h���X�g
	// ���ۂɕ`��R�}���h���i�[����Ƃ�
	ComPtr<ID3D12GraphicsCommandList> m_commandList;
	// �R�}���h�A���P�[�^��
	// �R�}���h���X�g�ɐςݍ��ނ��߂̃o�b�t�@���m�ۂ��邽�߂ɕK�v
	// �o�b�t�@���Ƃɗp��
	std::vector<ComPtr<ID3D12CommandAllocator>> m_commandAllocators;

	// �t�F���X
	HANDLE m_fenceWaitEvent;
	// �t�F���X
	// �o�b�t�@���Ƃɍ쐬
	std::vector<ComPtr<ID3D12Fence1>> m_frameFences;
	// �t�F���X�̒l
	// �o�b�t�@���Ƃɍ쐬
	std::vector<UINT64> m_frameFenceValues;

	// �r���[�|�[�g
	CD3DX12_VIEWPORT m_viewport;
	// �V�U�[��`
	CD3DX12_RECT m_scissorRect;

	std::vector<std::shared_ptr<RootSignature>> m_rootSignatures;

private: // �������֐�
	HRESULT EnableDebugLayer();
	HRESULT CreateDevice(); // �f�o�C�X�𐶐�
	HRESULT CreateCommandQueue(); // �R�}���h�L���[�𐶐�
	HRESULT CreateSwapChain(HWND hwnd); // �X���b�v�`�F�C���𐶐�
	HRESULT CreateCommandList(); // �R�}���h���X�g�ƃR�}���h�A���P�[�^�[�𐶐�
	HRESULT CreateFence(); // �t�F���X�𐶐�
	void CreateViewPort(); // �r���[�|�[�g�𐶐�
	void CreateScissorRect(); // �V�U�[��`�𐶐�
	void CreateRootSignatures();

private: // �`��Ɏg�p����I�u�W�F�N�g
	// �����_�[�^�[�Q�b�g
	// �����_�[�i�`��j����^�[�Q�b�g�i���̂܂�܁j
	// �o�b�t�@�̐������쐬
	std::vector<ComPtr<ID3D12Resource1>> m_renderTargets;
	UINT m_rtvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_heapRtv;

	// �f�v�X�X�e���V��
	// �`�揇�Ƃ������߂�̂ɕK�v
	UINT m_dsvDescriptorSize;
	ComPtr<ID3D12DescriptorHeap> m_heapDsv;
	ComPtr<ID3D12Resource1> m_depthBuffer;
private:
	void PrepareRenderTargetView();
	void CreateDepthBuffer();

public:
	UINT m_frameIndex;
	// �`��̗���
	// BeginRender > �e�`��I�u�W�F�N�g�̕`��R�}���h���s > EndRender

	// ���݂̃t���[���o�b�t�@�̎擾
	// �`��R�}���h�֘A�̃I�u�W�F�N�g�̃��Z�b�g
	// �J���[�o�b�t�@�Ɛ[�x�X�e���V���o�b�t�@�̏�����
	void BeginRender();
	// ���߂����߂̎��s
	// CPU��GPU�̓���
	void EndRender();

	// CPU��GPU����������܂ő҂�
	void WaitPreviousFrame();
};

