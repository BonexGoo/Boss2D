#pragma once
#include <boss.hpp>
#include "zm_class.hpp"

class ABActivity
{
public:
    static buffer Create(chars type);

public:
    virtual chars Type() const {return "";}
    virtual void Import(const Context& json);
    virtual void Export(Context& json) const;
    virtual ZMMatrix Value() const {return ZMMatrix();}
    virtual ZMMatrix AddValue(chars key, double& value) {return ZMMatrix();}
    virtual ZMMatrix RatedValue(double rate) const {return ZMMatrix();}
};
typedef Object<ABActivity> ABActivityObject;
typedef Array<ABActivityObject> ABActivities;

class ABActivityMove : public ABActivity
{
public:
    chars Type() const override {return "move";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    ZMMatrix Value() const override;
    ZMMatrix AddValue(chars key, double& value) override;
    ZMMatrix RatedValue(double rate) const override;

private:
    ZMVector mVector;
};

class ABActivityScale : public ABActivity
{
public:
    chars Type() const override {return "scale";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    ZMMatrix Value() const override;
    ZMMatrix AddValue(chars key, double& value) override;
    ZMMatrix RatedValue(double rate) const override;

private:
    ZMVector mVector;
};

class ABActivityRotateX : public ABActivity
{
public:
    chars Type() const override {return "rotate_x";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    ZMMatrix Value() const override;
    ZMMatrix AddValue(chars key, double& value) override;
    ZMMatrix RatedValue(double rate) const override;

private:
    double mRotateX {0};
};

class ABActivityRotateY : public ABActivity
{
public:
    chars Type() const override {return "rotate_y";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    ZMMatrix Value() const override;
    ZMMatrix AddValue(chars key, double& value) override;
    ZMMatrix RatedValue(double rate) const override;

private:
    double mRotateY {0};
};

class ABActivityRotateZ : public ABActivity
{
public:
    chars Type() const override {return "rotate_z";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    ZMMatrix Value() const override;
    ZMMatrix AddValue(chars key, double& value) override;
    ZMMatrix RatedValue(double rate) const override;

private:
    double mRotateZ {0};
};

class ABActivityMatrix : public ABActivity
{
public:
    chars Type() const override {return "matrix";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;
    ZMMatrix Value() const override;

public:
    void SetMatrix(const ZMMatrix& matrix);

private:
    ZMMatrix mMatrix;
};

class ABActivitySpeed : public ABActivity
{
public:
    chars Type() const override {return "speed";}
    void Import(const Context& json) override;
    void Export(Context& json) const override;

public:
    void SetSpeed(uint32 msec);
    uint32 Speed() const;

public:
    static const uint32 Default {100};

private:
    uint32 mMsec {Default};
};
