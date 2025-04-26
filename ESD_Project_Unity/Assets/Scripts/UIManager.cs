using UnityEngine;
using UnityUISample;

public class UIManager : MonoBehaviour
{
    [SerializeField] private SimpleButton _simpleButton;

    private void Start()
    {
        _simpleButton.AddOnClickAction(OnClickFunction);
    }

    private void OnClickFunction()
    {
        
    }
}
