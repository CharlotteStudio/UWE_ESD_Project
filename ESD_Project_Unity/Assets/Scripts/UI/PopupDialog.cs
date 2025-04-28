using System;
using TMPro;
using UnityEngine;
using UnityEngine.UI;

namespace UnityUISample
{
    public class PopupDialog : MonoBehaviour
    {
        private readonly Color ScreenImageDefaultColor = new Color(0f, 0f, 0f, 0.3f);
        
        [SerializeField] private string _defaultTitelText = "Message";
        
        [Header("Setting")]
        [SerializeField] private string _defaultYesText = "Yes";
        [SerializeField] private string _defaultNoText  = "No";
        [SerializeField] private FontStyles _fontStyles = FontStyles.Normal;

        [Header("Can null Setting")]
        [SerializeField] private TMP_FontAsset _textFont;
        [SerializeField] private AudioClip _yesButtonSound;
        [SerializeField] private AudioClip _noButtonSound;
        
        [Header("Animation Setting")]
        [SerializeField] private float _scaleStartSize = 0.7f;
        [SerializeField] private float _scaleCloseSize = 0.7f;
        [SerializeField] private float _scaleTime = 0.1f;
        [SerializeField] private float _fadeDuration = 0.1f;
        
        [Header("Text")]
        [SerializeField] private TMP_Text _titleText;
        [SerializeField] private TMP_Text _contentText;

        [Header("Screen Background")]
        [SerializeField] private Image _screenImage;
        [SerializeField] private Button _screenButton;
        
        [Header("Dialog Group")]
        [SerializeField] private Transform _dialogGroup;
        [SerializeField] private CanvasGroup _canvasGroup;
        [SerializeField] private SimpleButton _cancelButton;
        
        [Header("Yes / No Button Group")]
        [SerializeField] private GameObject _buttonGroup;
        [SerializeField] private SimpleButton _yesButton;
        [SerializeField] private SimpleButton _noButton;
        
        private System.Action _onCancelAction;
        private System.Action _onYesAction;
        private System.Action _onNoAction;
    
        private Coroutine _coroutine;
        
        public void SetActiveScreenButton(bool value) => _screenButton.interactable = value;
        public void SetScreenImageColor(Color color)  => _screenImage.color = color;
        public void SetScreenImageDefaultColor()      => _screenImage.color = ScreenImageDefaultColor;
        public void SetActiveButtonGroup(bool value)  => _buttonGroup.SetActive(value);
        public void SetTitleText(string str)          => _titleText.text = str;
        public void SetContentText(string str)        => _contentText.text = str;
        
        private void Start()
        {
            _screenButton.onClick.AddListener(OnClickCancel);
            _cancelButton.AddOnClickAction(OnClickCancel);
            _yesButton.AddOnClickAction(OnClickYes);
            _noButton.AddOnClickAction(OnClickNo);
        }

        private void OnDisable()
        {
        }
        
        public void StartPopupDialog(string content, string title = "",
            System.Action onCancelAction = null, System.Action onYesAction = null,System.Action onNoAction = null,
            float scaleTime = 0f, float fadeDuration = 0f)
        {
            SetActiveAllButton(true);
            
            SetTitleText(String.IsNullOrEmpty(title) ? _defaultTitelText : title);
            SetContentText(content);
            _onCancelAction = onCancelAction;
            _onYesAction = onYesAction;
            _onNoAction = onNoAction;
        
            SetActiveButtonGroup(!(onYesAction == null && onNoAction == null));
            
            gameObject.SetActive(true);
            
            if (scaleTime != 0)
                _scaleTime = scaleTime;
            
            if (_scaleTime != 0)
                _dialogGroup.AnimationLocalScale(_scaleStartSize, 1f, _scaleTime);

            if (fadeDuration != 0)
                _fadeDuration = fadeDuration;
            
            if (_fadeDuration != 0)
                _canvasGroup.AnimationFadeIn(_fadeDuration);
        }

        #region Private Function
    
        private void OnClickYes()
        {
            _onYesAction?.Invoke();
            DisableAnimation();
        }
    
        private void OnClickNo()
        {
            _onNoAction?.Invoke();
            DisableAnimation();
        }
    
        private void OnClickCancel()
        {
            _onCancelAction?.Invoke();
            DisableAnimation();
        }

        private void SetActiveAllButton(bool value)
        {
            _screenButton.interactable = value;
            _cancelButton.interactable = value;
            _yesButton.interactable = value;
            _noButton.interactable = value;
        }

        private void DisableAnimation()
        {
            _onYesAction = null;
            _onNoAction = null;
            _onCancelAction = null;
            
            SetActiveAllButton(false);
            
            if (_scaleTime != 0)
                _dialogGroup.AnimationLocalScale(1f, _scaleCloseSize, _scaleTime);
            
            if (_fadeDuration != 0)
                _canvasGroup.AnimationFadeOut(_fadeDuration);
            
            SimpleAnimationSample.Instance.AnimationEndAction(
                () =>
                {
                    if (gameObject == null) return;
                    gameObject.SetActive(false);
                }, _scaleTime);
        }
        
        #endregion
        
    }
}
